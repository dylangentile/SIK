#include "term.h"
#include "error.h"

Term::Term(TermID id) : mId(id), mDebugToken(nullptr)
{

}

Term::~Term() {}

void
Term::assignDebug(Token* debugTok)
{
	mDebugToken = debugTok;
}


SymbolPush::SymbolPush() : Term(kTerm_SymbolPush)
{

}

SymbolPush::~SymbolPush() {}


ValuePush::ValuePush() : Term(kTerm_ValuePush)
{
	mVal.u64 = 0;
}

ValuePush::~ValuePush() {}

void
ValuePush::parseValue(Token* tok)
{
	switch(tok->mType)
	{
		case kToken_I64:
		case kToken_I32:
		case kToken_I16:
		case kToken_I8:  mVal.i64 = std::stoll(tok->mStr);
		break;
		case kToken_U64:
		case kToken_U32:
		case kToken_U16:
		case kToken_U8:  mVal.u64 = std::stoull(tok->mStr);
		break;
		case kToken_STRING: mVal.strVal = tok->mStr;
		break;
		case kToken_BOOL: mVal.bVal = tok->mStr == "true" ? true : false;
		break;
		case kToken_FLOAT: mVal.fVal = std::stof(tok->mStr);
		break;
		case kToken_DOUBLE: mVal.dVal = std::stod(tok->mStr);
		break;
		default: lerror(kE_Fatal, tok, "Parser interpreted token as value, but cannot parse it!");
	}
}


TypePush::TypePush() : Term(kTerm_TypePush), mType(kType_NULL)
{

}

TypePush::~TypePush() {}


Operation::Operation() : Term(kTerm_Operation), op(kToken_NULL)
{

}

Operation::~Operation() {}


UnaryLHS::UnaryLHS() : Term(kTerm_UnaryLHS), op(kToken_NULL)
{

}

UnaryLHS::~UnaryLHS() {}


ExecutePrev::ExecutePrev() : Term(kTerm_ExecutePrev)
{

}

ExecutePrev::~ExecutePrev() {}


Execute::Execute(Token* tok) : Term(kTerm_Execute), state(nullptr)
{
	if(tok != nullptr)
	{
		if(tok->mCat != kCat_Symbol)
			lerror(kE_Fatal, tok, "Invalid tok passed to Execute! INTERNAL ERROR!");

		SymbolPush* s_state = new SymbolPush();
		s_state->symbol = tok->mStr;
		state = static_cast<Term*>(s_state);
	}
}

Execute::~Execute()
{
	delete state;
}


Ternary::Ternary() : Term(kTerm_Ternary)
{

}

Ternary::~Ternary() {}


LambdaDesc::LambdaDesc() : Term(kTerm_LambdaDesc), retType(kType_NULL)
{

}

LambdaDesc::~LambdaDesc() {}


LambdaBody::LambdaBody() : Term(kTerm_LambdaBody)
{

}

LambdaBody::~LambdaBody()
{
	for(Term* state : termVec)
		delete state;
}


SymbolPush*			
LambdaBody::generateSymbolPush(Token* tok)
{
	SymbolPush* push = new SymbolPush();
	push->symbol = tok->mStr;
	push->assignDebug(tok);

	termVec.push_back(static_cast<Term*>(push));
	return push;
}

ValuePush*			
LambdaBody::generateValuePush(Token* tok)
{
	ValuePush* push = new ValuePush();
	push->parseValue(tok);
	push->assignDebug(tok);

	termVec.push_back(static_cast<Term*>(push));
	return push;
}

TypePush*			
LambdaBody::generateTypePush(Token* tok)
{
	TypePush* push = new TypePush();
	push->mType = convert_toktype_typeenum(tok->mType);
	push->assignDebug(tok);

	termVec.push_back(static_cast<Term*>(push));
	return push;
}

Operation*
LambdaBody::generateOp(Token* tok)
{
	Operation* op = new Operation();
	op->op = tok->mType;
	op->assignDebug(tok);

	termVec.push_back(static_cast<Term*>(op));
	return op;
}

UnaryLHS*
LambdaBody::generateUnaryLHS(Token* tok)
{
	UnaryLHS* ulhs = new UnaryLHS();
	ulhs->op = tok->mType;
	ulhs->assignDebug(tok);

	termVec.push_back(static_cast<Term*>(ulhs));
	return ulhs;
}

ExecutePrev*
LambdaBody::generateExecutePrev(Token* tok)
{
	ExecutePrev* execprv = new ExecutePrev();
	execprv->assignDebug(tok);

	termVec.push_back(static_cast<Term*>(execprv));
	return execprv;
}

Execute*
LambdaBody::generateExecute(void)
{
	Execute* ex = new Execute();
	termVec.push_back(static_cast<Term*>(ex));
	return ex;
}

Execute*
LambdaBody::generateExecute(Token* tok)
{
	Execute* ex = new Execute(tok);
	ex->assignDebug(tok);

	termVec.push_back(static_cast<Term*>(ex));
	return ex;
}

Ternary*
LambdaBody::generateTernary(Token* tok)
{
	Ternary* tern = new Ternary();
	tern->assignDebug(tok);

	termVec.push_back(static_cast<Term*>(tern));
	return tern;
}

LambdaDesc*
LambdaBody::generateLambdaDesc(Token* tok)
{
	LambdaDesc* desc = new LambdaDesc();
	desc->assignDebug(tok);

	termVec.push_back(static_cast<Term*>(desc));
	return desc;
}

LambdaBody*
LambdaBody::generateBody(Token* tok)
{
	LambdaBody* body = new LambdaBody();
	body->assignDebug(tok);

	termVec.push_back(static_cast<Term*>(body));
	return body;
}




