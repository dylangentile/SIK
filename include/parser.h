#pragma once
#include "token.h"
#include "term.h"
#include <deque>

class Parser
{
public:
	Parser(File* theFile);
	~Parser();


	LambdaBody* parse();



private:
	Token* fetchToken();
	Token* lookAhead(size_t n);

	LambdaBody* currentBody();

	void parseLambdaDesc();
	void parseLPAREN();
	void parseBody();



private:
	File* mFile;

	std::vector<Token*>::iterator tokIt;
	Token* currentToken;
	Token m_nulltoken;

	std::deque<LambdaBody*> bodyDeque;
};