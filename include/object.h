#pragma once
#include <string>
#include "typing.h"
#include "term.h"

typedef enum
{
	kObject_Value,
	kObject_Type,
	kObject_Symbol,
	kObject_Lambda,
	kObject_LambdaDesc

}ObjectID;

class Object
{
protected:
	Object(ObjectID id);
public:
	virtual ~Object();

	const ObjectID mId;
};



class ValueObject : public Object, public Variant
{
public:
	ValueObject();
	ValueObject(const Variant&);
	ValueObject(const ValuePush&);
	
	~ValueObject();
};

class TypeObject : public Object, public Type
{
public:
	TypeObject(const Type&);
	TypeObject(const TypePush&);
	~TypeObject();
};

class SymbolObject : public Object, public SymbolPush
{
public:
	SymbolObject(const SymbolPush&);
	~SymbolObject();
};

class LambdaObjectDesc : public Object, public LambdaDesc
{
public:
	LambdaObjectDesc(const LambdaDesc&);
	~LambdaObjectDesc();
};


class LambdaObject : public Object
{
public:
	LambdaObject(const LambdaBody& body);
	~LambdaObject();

	std::vector<Term*> termVec;
	LambdaObjectDesc* mDescriptor;

};
