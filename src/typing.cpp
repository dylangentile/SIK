#include "typing.h"

TypeEnum convert_toktype_typeenum(TokenType type)
{
	switch(type)
	{
		case kToken_VOID: return kType_VOID;
		case kToken_BOOL: return kType_BOOL;
		case kToken_STRING: return kType_STRING;
		
		case kToken_I64: return kType_I64;
		case kToken_I32: return kType_I32;
		case kToken_I16: return kType_I16;
		case kToken_I8:  return kType_I8;

		case kToken_U64: return kType_U64;
		case kToken_U32: return kType_U32;
		case kToken_U16: return kType_U16;
		case kToken_U8:  return kType_U8;

		case kToken_FLOAT: return kType_FLOAT;
		case kToken_DOUBLE: return kType_DOUBLE;
		default: return kType_NULL;
	};

	return kType_NULL;
}