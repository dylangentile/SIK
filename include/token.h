#pragma once
#include <string>
#include <vector>
#include <cstdint>


typedef enum
{
	kCat_NULL = 0,
	kCat_Operator,
	kCat_Type,
	kCat_Value,
	kCat_Symbol,
	kCat_Unary_LHS
}TokenCategory;

// Don't change the order of this enum. I perform less than / greater than comparisons
// in parser.cpp

typedef enum
{
	kToken_NULL = 0,

	//types
	kToken_I64,
	kToken_I32,
	kToken_I16,
	kToken_I8,

	kToken_U64,
	kToken_U32,
	kToken_U16,
	kToken_U8,

	kToken_FLOAT,
	kToken_DOUBLE,

	kToken_BOOL,
	kToken_STRING,
	kToken_VOID,

	//arithmetic/assignment operators
	kToken_PLUS,
	kToken_MINUS,
	kToken_MULTIPLY,
	kToken_DIVIDE,
	kToken_MODULO,

	kToken_ASSIGN_EQUAL,
	kToken_DECL_EQUAL, // :=

	kToken_PLUS_EQUAL,
	kToken_MINUS_EQUAL,
	kToken_MULTIPLY_EQUAL,
	kToken_DIVIDE_EQUAL,
	kToken_MODULO_EQUAL,

	//logical operators
	kToken_LOGIC_EQUAL,
	kToken_LOGIC_NOT_EQUAL,
	kToken_LESS,
	kToken_GREATER,
	kToken_LESS_EQUAL,
	kToken_GREATER_EQUAL,

	kToken_LOGIC_AND,
	kToken_LOGIC_OR,

	//unary operators
	kToken_AMPERSAND,
	kToken_NOT,

	//ternary operator
	kToken_TERNARY,

	//language operators
	kToken_ARROW, //->
//	kToken_DOLLAR,
	kToken_COMMA,

	kToken_LPAREN,
	kToken_RPAREN,
	kToken_LBRACKET,
	kToken_RBRACKET,
	kToken_LCURLY,
	kToken_RCURLY,

}TokenType;



class File;


typedef struct Token
{
	uint32_t offset;
	uint32_t count;
	uint32_t lineNumber;
	File* parentFile;


	std::string mStr;
	TokenType mType;
	TokenCategory mCat;
}Token;




class File
{
public:
	File();
	~File();

	const char* path;
	std::string content;
	std::vector<Token*> tokenVector;
};

bool isUnaryOp(TokenType x);
bool isBinOp(TokenType x);





