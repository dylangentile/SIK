#include "object.h"

Object::Object(ObjectID id) : mId(id)
{

}

Object::~Object()
{

}

ValueObject::ValueObject() : Object(kObject_Value)
{
	mType = kType_NULL;
	u64 = 0;
}

ValueObject::ValueObject(const Variant& other) : Object(kObject_Value)
{
	mType = other.mType;
	strVal = other.strVal;
	u64 = other.u64;
}

ValueObject::ValueObject(const ValuePush& other) : Object(kObject_Value)
{
	mType = other.mVal.mType;
	strVal = other.mVal.strVal;
	u64 = other.mVal.u64;
}

ValueObject::~ValueObject() {}


TypeObject::TypeObject(const Type& other) : Object(kObject_Type), Type(other)
{

}

TypeObject::TypeObject(const TypePush& other) : Object(kObject_Type), Type(other.mType)
{

}

TypeObject::~TypeObject(){}


SymbolObject::SymbolObject(const SymbolPush& other) : Object(kObject_Symbol), SymbolPush(other)
{

}

SymbolObject::~SymbolObject() {}


LambdaObjectDesc::LambdaObjectDesc(const LambdaDesc& other) : Object(kObject_LambdaDesc)
{
	argTypes = other.argTypes;
	argSymbols = other.argSymbols;
	retType = other.retType;
}

LambdaObjectDesc::~LambdaObjectDesc() {}


LambdaObject::LambdaObject(const LambdaBody& body) : Object(kObject_Lambda), termVec(body.termVec),
													 mDescriptor(nullptr)
{

}

LambdaObject::~LambdaObject() {}