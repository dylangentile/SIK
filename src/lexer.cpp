#include "lexer.h"
#include "error.h"
#include <fstream>
#include <unordered_map>

File::File() : path(nullptr) {}
File::~File() 
{
	for(Token* tok : tokenVector)
		delete tok;
}

/*
I've probably written about 5 lexers now, and it is literally the most boring thing on earth,
since I know how the problem is solved. So a lot of this code is copied from other lexers I have
written. (specifically the DIO lexer)

*/


const std::unordered_map<std::string, TokenType> type_map = 
{
	{"int",  kToken_I64},
	{"uint", kToken_U64},

	{"i64", kToken_I64},
	//{"i32", kToken_I32},
	//{"i16", kToken_I16},
	//{"i8",  kToken_I8},

	{"u64", kToken_U64},
	//{"u32", kToken_U32},
	//{"u16", kToken_U16},
	//{"u8",  kToken_U8},

	//{"float", kToken_FLOAT},
	{"double", kToken_DOUBLE},

	{"bool", kToken_BOOL},
	{"string", kToken_STRING},
	{"void", kToken_VOID}


};


const std::unordered_map<std::string, TokenType> operator_map = 
{
	{"+", kToken_PLUS},
	{"-", kToken_MINUS},
	{"*", kToken_MULTIPLY},
	{"/", kToken_DIVIDE},
	{"%", kToken_MODULO},

	{"=", kToken_ASSIGN_EQUAL},
	{":=", kToken_DECL_EQUAL},

	{"+=", kToken_PLUS_EQUAL},
	{"-=", kToken_MINUS_EQUAL},
	{"*=", kToken_MULTIPLY_EQUAL},
	{"/=", kToken_DIVIDE_EQUAL},
	{"%=", kToken_MODULO_EQUAL},

	{"==", kToken_LOGIC_EQUAL},
	{"!=", kToken_LOGIC_NOT_EQUAL},
	{"<", kToken_LESS},
	{">", kToken_GREATER},
	{"<=", kToken_LESS_EQUAL},
	{">=", kToken_GREATER_EQUAL},

	{"&&", kToken_LOGIC_AND},
	{"||", kToken_LOGIC_OR},

	
	{"&", kToken_AMPERSAND},
	{"!", kToken_NOT},

	{"?", kToken_TERNARY},
	
	{"->", kToken_ARROW},
//	{"$", kToken_DOLLAR},
	{",", kToken_COMMA},

	

	{"(", kToken_LPAREN},
	{")", kToken_RPAREN},
	{"[", kToken_LBRACKET},
	{"]", kToken_RBRACKET},
	{"{", kToken_LCURLY},
	{"}", kToken_RCURLY}

	

};





bool isWhitespace(const char x)
{
	return x == '\n' || x == '\r'
		|| x == '\t' || x == '\0'
		|| x == ' ';
}


bool isNumber(const char x)
{
	return '0' <= x && x <= '9';
}

bool isAlpha(const char x)
{
	return ('a' <= x && x <= 'z')
		|| ('A' <= x && x <= 'Z');
}

bool isIdentifierChar(const char x)
{
	return isAlpha(x) || isNumber(x) || x == '_';
}


File* lexFile(const char* path)
{
	File* theFile = new File;

	theFile->path = path;
	
	{
		std::ifstream filestream(path);
		if(!filestream)
			lerror(kE_Fatal, nullptr, "Failed to open file!");

		theFile->content = std::string(std::istreambuf_iterator<char>(filestream), std::istreambuf_iterator<char>());
	}

	uint32_t currentLine = 1;
	uint32_t currentOffset = -1;
	uint32_t currentCount = 0;

	char c1 = 0, c2 = 0;

	auto fileIt = theFile->content.begin();

	auto fetchChar = [&]() -> void
	{
		c1 = *fileIt;

		if(fileIt + 1 == theFile->content.end())
		{
			if(c2 == 0) c1 = 0;
			c2 = 0;
			return;
		}

		c2 = *(fileIt + 1);

		fileIt++;
		currentCount++;
		currentOffset++;
		if(c1 == '\n') currentLine++;
	};


	auto fetchToken = [&]() -> Token*
	{
		while((isWhitespace(c1) || (c1 == '/' && c2 == '/')) && c1 != '\0')
		{
			while(isWhitespace(c1))
			{
				fetchChar();
				if(c1 == '\0')
					return nullptr;
			}

			while(c1 == '/' && c2 == '/' && c1 != '\0')
			{
				while(c1 != '\n' && c1 != '\0')
					fetchChar();

				fetchChar();
			}
		}

		if(c1 == '\0')
			return nullptr;

		currentCount = 0;

		Token* tok = new Token;
		tok->parentFile = theFile;
		tok->offset = currentOffset;
		tok->lineNumber = currentLine;

		if(isIdentifierChar(c1) && !isNumber(c1))
		{
			while(isIdentifierChar(c1))
			{
				tok->mStr.push_back(c1);
				fetchChar();
			}

			if(tok->mStr == "true" || tok->mStr == "false")
			{
				tok->mCat = kCat_Value;
				tok->mType = kToken_BOOL;
				return tok;
			}

			auto finder = type_map.find(tok->mStr);
			if(finder != type_map.end())
			{
				tok->mCat = kCat_Type;
				tok->mType = finder->second;
			}
			else
			{
				tok->mCat = kCat_Symbol;
				tok->mType = kToken_NULL;
			}

			return tok;
		}

		if(c1 == '0' && c2 == 'x')
		{
			fetchChar();
			fetchChar();
			
			while((isNumber(c1) || ('a' <= c1 && c1 <= 'f') || ('A' <= c1 && c1 <= 'F'))) 
			{
				tok->mStr.push_back(c1);
				fetchChar();
			}

			tok->mCat = kCat_Value;
			tok->mType = kToken_I64;

			return tok;
		}

		if(isNumber(c1))
		{
			while(isNumber(c1))
			{
				tok->mStr.push_back(c1);
				fetchChar();
			}

			if(c1 == '.')
			{
				tok->mStr.push_back(c1);
				fetchChar();
				
				while(isNumber(c1))
				{
					tok->mStr.push_back(c1);
					fetchChar();
				}

				tok->mCat = kCat_Value;
				tok->mType = kToken_DOUBLE;
			}
			else
			{
				tok->mCat = kCat_Value;
				tok->mType = kToken_I64;
			}

			return tok;
		}

		{
			tok->mStr.clear();
			tok->mStr.push_back(c1);
			tok->mStr.push_back(c2);
			

			auto finder = operator_map.find(tok->mStr);

			if(finder != operator_map.end())
			{
				tok->mCat = kCat_Operator;
				tok->mType = finder->second;
				fetchChar();
				fetchChar();
				return tok;
			}
			else
			{
				tok->mStr.clear();
				tok->mStr.push_back(c1);

				if((finder = operator_map.find(tok->mStr)) != operator_map.end())
				{
					tok->mCat = kCat_Operator;
					tok->mType = finder->second;
					fetchChar();
					return tok;
				}


			}

			tok->mStr.clear();
		}

		//todo: support escapes!
		if(c1 == '\"')
		{
			fetchChar();
			while(c1 != '\"' && c1 != '\0')
			{
				tok->mStr.push_back(c1);
				fetchChar();
			}
			fetchChar();

			tok->mCat = kCat_Value;
			tok->mType = kToken_STRING;

			return tok;
		}

		tok->mStr.push_back(c1);
		tok->mCat = kCat_NULL;
		return tok;


	};

	fetchChar();
	currentOffset = 0;
	Token* theToken = nullptr;

	while(true)
	{
		theToken = fetchToken();
		if(theToken == nullptr)
			break;

		theToken->count = currentCount;
		if(theToken->mCat == kCat_NULL)
			lerror(kE_Fatal, theToken, "Unknown token!");

		if(isUnaryOp(theToken->mType) && c1 != ' ') //ie &a
			theToken->mCat = kCat_Unary_LHS;
		

		theFile->tokenVector.push_back(theToken);
	}

	return theFile;

}















