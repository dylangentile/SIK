#pragma once
#include <cstdint>
#include <string>
#include "token.h"

typedef enum
{
	kType_NULL = 0,
	kType_VOID,
	kType_BOOL,
	kType_STRING,

	kType_I64,
	kType_I32,
	kType_I16,
	kType_I8,

	kType_U64,
	kType_U32,
	kType_U16,
	kType_U8,

	kType_FLOAT,
	kType_DOUBLE

}TypeEnum;


typedef struct Variant
{
	TypeEnum mType;

	//I would add const char* to the union but then I have to manage memory.
	std::string strVal;
	union
	{
		int64_t		i64;
		int32_t		i32;
		int16_t		i16;
		int8_t		i8;

		uint64_t	u64;
		uint32_t	u32;
		uint16_t	u16;
		uint8_t		u8;

		float		fVal;
		double		dVal;

		bool		bVal;
		

	};
}Variant;

TypeEnum convert_toktype_typeenum(TokenType);