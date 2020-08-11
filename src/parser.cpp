#include "parser.h"
#include "error.h"
#include <string.h>


bool
isBinOp(TokenType x)
{
	return (kToken_PLUS <= x && x <= kToken_LOGIC_OR);
}

bool
isUnaryOp(TokenType x)
{
	return (x == kToken_AMPERSAND) || (x == kToken_NOT);
}

Parser::Parser(File* theFile) : mFile(theFile)
{
	tokIt = mFile->tokenVector.begin();
	currentToken = *tokIt;
	memset(&m_nulltoken, 0, sizeof(m_nulltoken));
}

Parser::~Parser()
{

}


Token*
Parser::fetchToken()
{
	if(currentToken == &m_nulltoken)
		return currentToken;
	
	tokIt++;

	if(tokIt == mFile->tokenVector.end())
		currentToken = &m_nulltoken;
	else
		currentToken = *tokIt;

	return currentToken;
}

Token*
Parser::lookAhead(size_t n)
{
	if(tokIt + n != mFile->tokenVector.end())
		return *(tokIt + n);
	return &m_nulltoken;
}

LambdaBody*
Parser::currentBody()
{
	return bodyDeque.back();
}

void
Parser::parseLambdaDesc()
{
	if(currentToken->mType != kToken_LPAREN)
		lerror(kE_Fatal, currentToken, "Invalid lambda descriptor!");

	LambdaDesc* desc = currentBody()->generateLambdaDesc(currentToken);

	fetchToken();

	bool hitVoid = false;
	while(true)
	{
		if(currentToken->mCat != kCat_Type)
			lerror(kE_Fatal, currentToken, "expected type!");

		TypeEnum theType = convert_toktype_typeenum(currentToken->mType);
		TypeAttribute theAttr = kAttr_NULL;
		
		fetchToken();

		if(theType == kType_VOID)
		{
			hitVoid = true;
			goto commaCheck;
		}

		switch(currentToken->mType)
		{
			case kToken_AMPERSAND:
			{ 
				theAttr = kAttr_Refrence;
				fetchToken();
			}
			break;
			case kToken_NOT:
			{
				theAttr = kAttr_Restrict;
				fetchToken();
			}
			break;
			default:;

		}

		desc->argTypes.push_back(Type(theType, theAttr));
		

		if(currentToken->mCat != kCat_Symbol)
			lerror(kE_Fatal, currentToken, "expected symbol!");

		desc->argSymbols.push_back(currentToken->mStr);

		fetchToken();

	commaCheck:
		if(currentToken->mType != kToken_COMMA)
			break;

		if(hitVoid)
			lerror(kE_Fatal, currentToken, "cannot have void followed by other types!");

		fetchToken();

	}

	if(currentToken->mType != kToken_ARROW)
		lerror(kE_Fatal, currentToken, "expected arrow!");

	fetchToken();

	if(currentToken->mCat != kCat_Type)
		lerror(kE_Fatal, currentToken, "expected type!");

	desc->retType = convert_toktype_typeenum(currentToken->mType);

	fetchToken();

	if(currentToken->mType != kToken_RPAREN)
		lerror(kE_Fatal, currentToken, "expected rparen: )");

	fetchToken();
}

void
Parser::parseLPAREN()
{
	if(currentToken->mType != kToken_LPAREN)
		lerror(kE_Fatal, currentToken, "Expected: (");

	Token* future = lookAhead(1);

	if(future->mType == kToken_RPAREN)
	{
		currentBody()->generateExecutePrev(currentToken);
		fetchToken();
		fetchToken();

		return;
	}
	else if(future->mCat == kCat_Symbol && lookAhead(2)->mType == kToken_RPAREN)
	{
		currentBody()->generateExecute(future);
		fetchToken();
		fetchToken();
		fetchToken();
	}
	else if(future->mCat == kCat_Type)
	{
		parseLambdaDesc();
	}
	else if(future->mType == kToken_LCURLY)
	{
		LambdaBody* nestedBody = new LambdaBody();
		nestedBody->assignDebug(future);

		Execute* ex = currentBody()->generateExecute();
		ex->assignDebug(currentToken);
		fetchToken(); // past lparen
		
		bodyDeque.push_back(nestedBody);
		fetchToken(); //fetch past lcurly
		parseBody();
		bodyDeque.pop_back();

		if(currentToken->mType != kToken_RCURLY)
			lerror(kE_Fatal, currentToken, "Expected rcurly: )");

		ex->state = static_cast<Term*>(nestedBody);
		fetchToken();

	}
	else
	{
		lerror(kE_Fatal, currentToken, "Invalid following tokens!");
	}


}

//enters token after '{'
void
Parser::parseBody()
{
	Token* prevToken = currentToken;
	while(currentToken != &m_nulltoken)
	{
		prevToken = currentToken;

		if(currentToken->mType == kToken_RCURLY)
		{
			fetchToken();
			break;
		}

		switch(currentToken->mCat)
		{
			case kCat_NULL: lerror(kE_Fatal, currentToken, "Unknown Token!");
			break;
			case kCat_Operator:
			{
				//I could use a switch here but this seems neater to me
				if(isBinOp(currentToken->mType) || isUnaryOp(currentToken->mType))
				{
					currentBody()->generateOp(currentToken);
					fetchToken();
				}
				else if(currentToken->mType == kToken_TERNARY)
				{
					currentBody()->generateTernary(currentToken);
					fetchToken();
				}
				else if(currentToken->mType == kToken_LPAREN)
				{
					parseLPAREN();
				}
				else if(currentToken->mType == kToken_LCURLY)
				{
					LambdaBody* nestedBody = currentBody()->generateBody(currentToken);
					bodyDeque.push_back(nestedBody);
					fetchToken(); //fetch past lcurly
					parseBody();
					bodyDeque.pop_back();
				}
				else if(currentToken->mType == kToken_LBRACKET)
				{
					//todo:
					lerror(kE_Fatal, currentToken, "arrays currently unsupported!");
				}
				else
				{
					lerror(kE_Fatal, currentToken, "operator used incorrectly!");
				}
			}
			break;
			case kCat_Type:
			{
				currentBody()->generateTypePush(currentToken);
				fetchToken();
			}
			break;
			case kCat_Value:
			{
				currentBody()->generateValuePush(currentToken);
				fetchToken();
			}
			break;
			case kCat_Symbol: 
			{
				currentBody()->generateSymbolPush(currentToken);
				fetchToken();
			}
			break;
			case kCat_Unary_LHS:
			{
				currentBody()->generateUnaryLHS(currentToken);
				fetchToken();
			}
			break;

		}


		if(prevToken == currentToken)
			lerror(kE_Fatal, currentToken, "Unhandled Token!");
	}

	return;
}

LambdaBody*
Parser::parse()
{
	LambdaBody* globalBody = new LambdaBody();
	bodyDeque.push_back(globalBody);
	
	parseBody();
	
	return globalBody;
}