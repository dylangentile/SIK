#include "interpreter.h"
#include "error.h"
#include <cstdio>


StackFrame::StackFrame(LambdaObject* body) : mBody(body)
{

}

StackFrame::~StackFrame()
{
	/*for(Object* obj : theDeque)
		delete obj;

	for(auto pair : symbolMap)
		delete pair.second;*/
}





Interpreter::Interpreter(LambdaBody* gbody) : globalBody(gbody)
{

}

Interpreter::~Interpreter() 
{
	
}

StackFrame*
Interpreter::currentFrame()
{
	return stackFrameDeque.back();
}

/* //some beautifully complicated code ;)
Object*
Interpreter::derefrence(Object* obj)
{
	if(obj->mId == kObject_Symbol)
	{
		std::function<Object*(SymbolObject*, decltype(stackFrameDeque)::reverse_iterator&)>
		findDeref = [&](SymbolObject* lookup, decltype(stackFrameDeque)::reverse_iterator& dIt) 
			-> Object*
		{
			for(; dIt != stackFrameDeque.rend(); dIt++)
			{
				auto finder = (*dIt)->symbolMap.find(lookup->symbol);
				if(finder != (*dIt)->symbolMap.end())
				{
					Object* retObj = finder->second;
					
					if(retObj->mId != kObject_Symbol)
						return retObj;

					return findDeref(static_cast<SymbolObject*>(retObj), dIt);

				}
			}

			return nullptr;
		};

		auto it = stackFrameDeque.rbegin();
		return findDeref(static_cast<SymbolObject*>(obj), it);
	}
	else
	{
		return obj;
	}
}
*/


Object*
Interpreter::derefrence(Object* obj)
{
	if(obj->mId == kObject_Symbol)
	{
		SymbolObject* s_obj = static_cast<SymbolObject*>(obj);

		for(auto it = stackFrameDeque.rbegin(); it != stackFrameDeque.rend(); it++)
		{
		checkSOBJ:
			auto finder = (*it)->symbolMap.find(s_obj->symbol);
			if(finder != (*it)->symbolMap.end())
			{
				Object* retObj = finder->second;
				
				if(retObj->mId != kObject_Symbol)
					return retObj;

				s_obj = static_cast<SymbolObject*>(retObj);
				goto checkSOBJ;
			}

		}

		return static_cast<Object*>(s_obj); //undefined symbol
	}
	else
	{
		return obj;
	}

	return nullptr;
}

Object*
Interpreter::popDeque(Term* debug)
{
	auto& theDeque = currentFrame()->theDeque;

	if(theDeque.size() < 1)
		lerror(kE_Fatal, debug == nullptr ? nullptr :debug->mDebugToken, 
				"Stack is missing terms for this operation");

	Object* retVal = theDeque.back();
	theDeque.pop_back();

	return retVal;
}

bool 
isSIntType(TypeEnum x)
{
	return kType_I64 <= x && x <= kType_I8;
}

bool
isUIntType(TypeEnum x)
{
	return kType_U64 <= x && x <= kType_U8;
}

bool
isIntType(TypeEnum x)
{
	return isSIntType(x) || isUIntType(x);
}


#define BINOP_MACRO_NO_MOD(member) do{ \
	switch(op->op) { \
		case kToken_PLUS: result->member = LHS->member + RHS->member; \
		break; \
		case kToken_MINUS: result->member = LHS->member - RHS->member; \
		break; \
		case kToken_MULTIPLY: result->member = LHS->member * RHS->member; \
		break; \
		case kToken_DIVIDE: result->member = LHS->member / RHS->member; \
		break; \
		case kToken_LOGIC_EQUAL:{result->bVal = LHS->member == RHS->member; \
			result->mType = kType_BOOL;}\
		break; \
		case kToken_LOGIC_NOT_EQUAL:{result->bVal = LHS->member != RHS->member; \
			result->mType = kType_BOOL;}\
		break; \
		case kToken_LESS:{result->bVal = LHS->member < RHS->member; \
			result->mType = kType_BOOL;}\
		break; \
		case kToken_GREATER:{result->bVal = LHS->member > RHS->member; \
			result->mType = kType_BOOL;}\
		break; \
		case kToken_LESS_EQUAL:{result->bVal = LHS->member <= RHS->member; \
			result->mType = kType_BOOL;}\
		break; \
		case kToken_GREATER_EQUAL:{result->bVal = LHS->member >= RHS->member; \
			result->mType = kType_BOOL;}\
		break; \
		default: break;}}while(0)

#define BINOP_MACRO(member) do{{ \
	if(op->op == kToken_MODULO) \
	{\
		result->member = LHS->member % RHS->member; \
	}\
	else\
	{\
		BINOP_MACRO_NO_MOD(member);\
	}\
	}}while(0)





Object*
Interpreter::standardBinOp(ValueObject* LHS, ValueObject* RHS, Operation* op)
{
	if((LHS->mType == kType_BOOL || RHS->mType == kType_BOOL) 
		&& (op->op != kToken_LOGIC_EQUAL
		|| op->op != kToken_LOGIC_NOT_EQUAL))
		lerror(kE_Fatal, op->mDebugToken, "cannot perform binary operation on boolean!");

	if((LHS->mType == kType_STRING || RHS->mType == kType_STRING)
		&& (op->op != kToken_PLUS
		||  op->op != kToken_LOGIC_EQUAL
		|| 	op->op != kToken_LOGIC_NOT_EQUAL))
		lerror(kE_Fatal, op->mDebugToken, "cannot perform non-addition operation on string!");

	if(LHS->mType != RHS->mType)
	{
		if(isIntType(LHS->mType) && isIntType(RHS->mType))
		{
			if(isUIntType(LHS->mType) && isUIntType(RHS->mType))
			{
				RHS->u64 = (uint64_t)RHS->i64;
				RHS->mType = kType_U64;
			}
			else
			{
				LHS->u64 = (uint64_t)LHS->i64;
				LHS->mType = kType_U64;
			}
		}
		else
		{
			lerror(kE_Fatal, op->mDebugToken, "cannot perform operation on differing types!");
		}
	}

	if(op->op == kToken_MODULO && (LHS->mType == kType_DOUBLE || LHS->mType == kType_FLOAT))
		lerror(kE_Fatal, op->mDebugToken, "cannot perform '%' on a floating point value!");

	ValueObject* result = new ValueObject();
	result->mType = LHS->mType;

	switch(LHS->mType)
	{
		case kType_STRING: 
		switch(op->op)
		{
			case kToken_PLUS: result->strVal = LHS->strVal + RHS->strVal;
			break;
			case kToken_LOGIC_EQUAL: 	{result->bVal = LHS->strVal == RHS->strVal;
										 result->mType = kType_BOOL;}
			break;
			case kToken_LOGIC_NOT_EQUAL:{result->bVal = LHS->strVal != RHS->strVal;
										 result->mType = kType_BOOL;} 
			break;
			default: lerror(kE_Fatal, op->mDebugToken, 
							"cannot perform operation on string!");
			break;
		}
		break;
		case kType_BOOL: 
		switch(op->op)
			{
				case kToken_LOGIC_EQUAL:	{result->bVal = LHS->bVal == RHS->bVal;
											 result->mType = kType_BOOL;} 
				break;
				case kToken_LOGIC_NOT_EQUAL:{result->bVal = LHS->bVal != RHS->bVal;
											 result->mType = kType_BOOL;} 
				break;
				default: lerror(kE_Fatal, op->mDebugToken, 
								"cannot perform operation on bool!");
				break;
			}
		break;
		case kType_U64: BINOP_MACRO(u64);
		break;
		case kType_I64: BINOP_MACRO(i64);
		break;
		case kType_DOUBLE: BINOP_MACRO_NO_MOD(dVal);
		break;
		case kType_FLOAT: BINOP_MACRO_NO_MOD(fVal);
		break;
		default: lerror(kE_Fatal, op->mDebugToken, "unsupported types for binop!");
		break;
	}



	return result;
}

/*Object*
Interpreter::logicalBinOp(ValueObject* LHS, ValueObject* RHS, Operation* op)
{

}*/


inline TokenType
convertCompound(TokenType x)
{
	switch(x)
	{
		case kToken_PLUS_EQUAL: return kToken_PLUS;
		case kToken_MINUS_EQUAL: return kToken_MINUS;
		case kToken_MULTIPLY_EQUAL: return kToken_MULTIPLY;
		case kToken_DIVIDE_EQUAL: return kToken_DIVIDE;
		case kToken_MODULO_EQUAL: return kToken_MODULO;
		default: return kToken_NULL;
	}
}

//todo:
//whenever things are popped, or replaced, ie with assignment, 
//we are probably creating a memory leak.
// deal with this promptly!
//luckily this function and the interpretframe functions are the sole
// problems

void 
Interpreter::interpretOperation(Operation* op)
{
	if(isUnaryOp(op->op))
	{
		//todo:
		lerror(kE_Fatal, op->mDebugToken, "unary operations unsupported!");
	}
	else
	{

		StackFrame* cFrame = currentFrame();
		
		Object* RHS = popDeque(static_cast<Term*>(op));
		Object* LHS = popDeque(static_cast<Term*>(op));

		Object* oRHS = RHS, *oLHS = LHS;
		
		

		Object* result = nullptr;
		switch(op->op)
		{
			case kToken_LOGIC_EQUAL:
			case kToken_LOGIC_NOT_EQUAL:
			case kToken_LESS:
			case kToken_GREATER:
			case kToken_LESS_EQUAL:
			case kToken_GREATER_EQUAL:

			case kToken_PLUS:
			case kToken_MINUS:
			case kToken_MULTIPLY:
			case kToken_DIVIDE:
			case kToken_MODULO: 
			{
				RHS = derefrence(RHS);
				LHS = derefrence(LHS);

				if(LHS->mId != kObject_Value || RHS->mId != kObject_Value)
					lerror(kE_Fatal, op->mDebugToken, 
						"cannot perform this operation on non-value term!");

				result = standardBinOp( static_cast<ValueObject*>(LHS), 
										static_cast<ValueObject*>(RHS), op);

				cFrame->theDeque.push_back(result);
				delete RHS;
				delete LHS;

			}
			break;
			case kToken_ASSIGN_EQUAL:
			{
				RHS = derefrence(RHS);

				if(LHS->mId == kObject_Symbol)
				{
					SymbolObject* sym = static_cast<SymbolObject*>(LHS);
					for(StackFrame* sf : stackFrameDeque)
					{
						auto finder = sf->symbolMap.find(sym->symbol);
						if(finder != sf->symbolMap.end())
						{
							delete finder->second;
							finder->second = RHS;
							cFrame->theDeque.push_back(LHS);
						}
					}

					lerror(kE_Fatal, op->mDebugToken, "LHS assignee symbol undefined");
				}
				else if(LHS->mId == kObject_Lambda && RHS->mId == kObject_LambdaDesc)
				{
					LambdaObject* lobj = static_cast<LambdaObject*>(LHS);
					lobj->mDescriptor = static_cast<LambdaObjectDesc*>(RHS);
					cFrame->theDeque.push_back(LHS);

				}
				else
				{
					lerror(kE_Fatal, op->mDebugToken, "cannot perform assignment on LHS");
				}
			}
			break;
			case kToken_DECL_EQUAL:
			{
				RHS = derefrence(RHS);

				if(LHS->mId != kObject_Symbol)
					lerror(kE_Fatal, op->mDebugToken, ":= operator requires symbol lhs");

				SymbolObject* sym = static_cast<SymbolObject*>(LHS);
				auto finder = cFrame->symbolMap.find(sym->symbol);

				if(finder != cFrame->symbolMap.end())
					lerror(kE_Fatal, op->mDebugToken, 
						"lhs symbol already defined in current frame!");

				if(RHS->mId == kObject_Type)
				{
					ValueObject* val = new ValueObject();
					val->mType = static_cast<TypeObject*>(RHS)->mTypeId;
					delete RHS;
					RHS = static_cast<Object*>(val);
				}

				cFrame->symbolMap.insert(
					std::make_pair(sym->symbol, RHS));
				cFrame->theDeque.push_back(LHS);
				

			}
			break;
			case kToken_PLUS_EQUAL:
			case kToken_MINUS_EQUAL:
			case kToken_MULTIPLY_EQUAL:
			case kToken_DIVIDE_EQUAL:
			case kToken_MODULO_EQUAL:
			{
				if(LHS->mId != kObject_Symbol)
					lerror(kE_Fatal, op->mDebugToken, "compound operation requires symbol lhs");

				SymbolObject* sym = static_cast<SymbolObject*>(LHS);
				SymbolObject* tempLHS = new SymbolObject(*static_cast<SymbolPush*>(sym));
				Operation theOp;

				cFrame->theDeque.push_back(LHS);
				cFrame->theDeque.push_back(tempLHS); //deletes tempLHS for us
				cFrame->theDeque.push_back(RHS);
				theOp.op = convertCompound(op->op);
				interpretOperation(&theOp);

				theOp.op = kToken_ASSIGN_EQUAL;
				interpretOperation(&theOp);

			}
			break;
			case kToken_LOGIC_AND:
			case kToken_LOGIC_OR:
			{
				RHS = derefrence(RHS);
				LHS = derefrence(LHS);

				if(LHS->mId != kObject_Value || RHS->mId != kObject_Value)
					lerror(kE_Fatal, op->mDebugToken, 
						"cannot perform this operation on non-value term!");

				ValueObject* lval = static_cast<ValueObject*>(LHS);
				ValueObject* rval = static_cast<ValueObject*>(RHS);

				if(lval->mType != kType_BOOL || rval->mType != kType_BOOL)
					lerror(kE_Fatal, op->mDebugToken, 
						"cannot perform &&/|| on non-boolean values");

				if(op->op == kToken_LOGIC_AND)
					lval->bVal = lval->bVal && rval->bVal;
				else
					lval->bVal = lval->bVal || rval->bVal;

				cFrame->theDeque.push_back(LHS);
				delete RHS;
			}
			break;
			default: lerror(kE_Fatal, op->mDebugToken, "invalid/unsupported operation token!");
			break;
		}


		//cleanup derefrence
		if(oRHS != RHS)
			delete oRHS;
		if(oLHS != LHS)
			delete oLHS;
	

	
		
		


	}
}



void
Interpreter::interpretPrint()
{
	Object* formatStrObj = popDeque();
	formatStrObj = derefrence(formatStrObj);
	if(formatStrObj->mId != kObject_Value)
	{
		oerror(kE_Fatal, formatStrObj, "print doesn't support this object!");
	}
	else
	{
		std::string formatStr = "";
		ValueObject* val = static_cast<ValueObject*>(formatStrObj);
		
		switch(val->mType)
		{

			case kType_STRING: printf("%s\n", val->strVal.c_str());
			break;
			case kType_U64: printf("%llu\n", val->u64);
			break;
			case kType_I64:	printf("%lli\n", val->i64);
			break;
			default: oerror(kE_Fatal, val, "print doesn't support this type!");
		}

	}

}

#define TERM_CONVERT_PUSH(term_type, obj_type) do{\
	term_type* push = static_cast<term_type*>(term);\
	obj_type* obj = new obj_type(*push);\
	frame->theDeque.push_back(static_cast<Object*>(obj));\
}while(0)


void 
Interpreter::interpretTerm(Term* term, StackFrame* frame)
{
	switch(term->mId)
	{
		case kTerm_SymbolPush:
		{
			TERM_CONVERT_PUSH(SymbolPush, SymbolObject);
		}
		break;
		case kTerm_ValuePush:
		{
			TERM_CONVERT_PUSH(ValuePush, ValueObject);
		}
		break;
		case kTerm_TypePush:
		{
			TERM_CONVERT_PUSH(TypePush, TypeObject);
		}
		break;
		case kTerm_Operation: interpretOperation(static_cast<Operation*>(term));
		break;
		case kTerm_UnaryLHS: lerror(kE_Fatal, term->mDebugToken, "currently unsupported");
		break;
		case kTerm_ExecutePrev:
		case kTerm_Execute:
		{
		
			if(term->mId == kTerm_Execute)
				interpretTerm(static_cast<Execute*>(term)->state, frame);
			
			Object* expr = popDeque(term);
			//needs garbage collection

			if(	   expr->mId == kObject_Symbol 
				&& static_cast<SymbolObject*>(expr)->symbol == "print")
			{
				interpretPrint();
				break;
			}


			expr = derefrence(expr);

			if(expr->mId != kObject_Lambda)
				lerror(kE_Fatal, term->mDebugToken, "expected lambda body to execute!");

			LambdaObject* lambda = static_cast<LambdaObject*>(expr);
			LambdaObjectDesc* descriptor = lambda->mDescriptor;

			if(descriptor->argTypes.size() != descriptor->argSymbols.size())
				lerror(kE_Fatal, term->mDebugToken, "ill-formed lambda!");


			StackFrame nSF(lambda);

			for(int i = descriptor->argTypes.size() - 1; i >= 0 ; i--)
			{
				Object* arg = popDeque(term);
				arg = derefrence(arg);
				if(arg->mId != kObject_Value)
					lerror(kE_Fatal, term->mDebugToken, "non-value arguements are currently unsupported!");

				ValueObject* val = static_cast<ValueObject*>(arg);
				
				Type argType(descriptor->argTypes[i]);

				if(val->mType != argType.mTypeId)
					lerror(kE_Fatal, term->mDebugToken, "mismatched arguement types!"); 
					//todo update error message

				//todo: handle refrenceObject on the outside and restrict

				if(argType.mAttr == kAttr_Refrence)
				{
					//pass by refrence
					nSF.symbolMap.insert(std::make_pair(descriptor->argSymbols[i], 
						static_cast<Object*>(val)));
				}
				else
				{
					//pass by copy
					nSF.symbolMap.insert(std::make_pair(descriptor->argSymbols[i], 
						new ValueObject(*static_cast<Variant*>(val))));
					//todo: implement interpreter garbage collector!
				}
			}

			stackFrameDeque.push_back(&nSF);
			interpretFrame();
			stackFrameDeque.pop_back();

			
			


			
		}
		break;
		case kTerm_Ternary:
		break;
		case kTerm_LambdaDesc:
		{
			TERM_CONVERT_PUSH(LambdaDesc, LambdaObjectDesc);
		}
		break;
		case kTerm_LambdaBody:
		{
			TERM_CONVERT_PUSH(LambdaBody, LambdaObject);
		}
		break;
		default: lerror(kE_Fatal, term->mDebugToken, "Un-categorizable token!");
		break;
	}
}


void
Interpreter::interpretFrame()
{
	StackFrame* frame = currentFrame();
	for(Term* term : frame->mBody->termVec)
		interpretTerm(term, frame);

	auto finder = frame->symbolMap.find("main");
	if(finder != frame->symbolMap.end())
	{
		frame->theDeque.push_back(finder->second);
		ExecutePrev prevexec;
		interpretTerm(&prevexec, frame);
	}
}


void
Interpreter::interpret()
{	
	LambdaObject lobj(*globalBody);
	StackFrame gSF(&lobj);

	stackFrameDeque.push_back(&gSF);
	interpretFrame();
	stackFrameDeque.pop_back();	
}