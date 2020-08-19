#pragma once
#include "token.h"



typedef enum
{
	kE_Warning,
	kE_Error,
	kE_Fatal
}Error_Level;


void printErrors();
void lerror(Error_Level level, Token* tok, const char* message);
#define oerror(level, obj, message) lerror(level, nullptr, message)