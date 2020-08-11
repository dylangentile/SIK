#pragma once
#include "token.h"
#include "typing.h"
#include <string>
#include <vector>

typedef enum
{
	kTerm_NULL = 0,
	kTerm_SymbolPush,
	kTerm_ValuePush,
	kTerm_TypePush,
	kTerm_Operation,
	kTerm_UnaryLHS,
	kTerm_ExecutePrev,
	kTerm_Execute,
	kTerm_Ternary,
	kTerm_LambdaDesc,
	kTerm_LambdaBody
}TermID;


class Term
{
protected:
	Term(TermID id);
public:
	~Term();
	void assignDebug(Token* debugTok);

	const TermID mId;
	Token* mDebugToken;

};



class SymbolPush : public Term
{
public:
	SymbolPush();
	~SymbolPush();

	std::string symbol;

};


class ValuePush : public Term
{
public:
	ValuePush();
	~ValuePush();

	Variant mVal;

	void parseValue(Token*);

	

};

class TypePush : public Term
{
public:
	TypePush();
	~TypePush();

	TypeEnum mType;
};

class Operation : public Term
{
public:
	Operation();
	~Operation();

	TokenType op;


};

class UnaryLHS : public Term
{
public:
	UnaryLHS();
	~UnaryLHS();

	TokenType op;
};

//In the interpreter the disctinction betweeen Execute and ExecutePrev "matters"
//but in the analyzer this becomes an Execute
class ExecutePrev : public Term
{
public:
	ExecutePrev();
	~ExecutePrev();
};

class Execute : public Term
{
public:
	Execute(Token* tok = nullptr);
	~Execute();

	Term* state;


};

class Ternary : public Term
{
public:
	Ternary();
	~Ternary();

};

class LambdaDesc : public Term
{
public:
	LambdaDesc();
	~LambdaDesc();

	std::vector<TypeEnum> argTypes;
	std::vector<std::string> argSymbols;

	TypeEnum retType;

};


class LambdaBody : public Term
{
public:
	LambdaBody();
	~LambdaBody();

	SymbolPush*			generateSymbolPush(Token*);
	ValuePush*			generateValuePush(Token*);
	TypePush*			generateTypePush(Token*);
	Operation* 			generateOp(Token*);
	UnaryLHS*			generateUnaryLHS(Token*);
	ExecutePrev*		generateExecutePrev(Token*);
	Execute*			generateExecute(void);
	Execute*			generateExecute(Token*);
	Ternary* 			generateTernary(Token*);
	LambdaDesc*			generateLambdaDesc(Token*);
	LambdaBody* 		generateBody(Token*);
	
	
public:
	std::vector<Term*> termVec;


};




















