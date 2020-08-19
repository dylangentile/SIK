#pragma once
#include "term.h"
#include "object.h"
#include <unordered_map>
#include <deque>

class StackFrame
{
public:
	StackFrame(LambdaObject*);
	~StackFrame();
	
	std::unordered_map<std::string, Object*> symbolMap;
	LambdaObject* const mBody;

	std::deque<Object*> theDeque;

};


class Interpreter
{
public:
	Interpreter(LambdaBody*);
	~Interpreter();

	void interpret();

private:
	StackFrame* currentFrame();
	Object* popDeque(Term* debug = nullptr);

	Object* derefrence(Object*);
	Object* standardBinOp(ValueObject*, ValueObject*, Operation*);
	void interpretOperation(Operation*);
	void interpretPrint();
	void interpretTerm(Term*, StackFrame*);
	void interpretFrame();

	void val_cast(ValueObject*, TypeObject*);


private:
	LambdaBody* const globalBody;
	std::deque<StackFrame*> stackFrameDeque;

};