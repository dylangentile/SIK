#include "error.h"
#include "lexer.h"
#include "parser.h"
#include <cstdio>

static_assert(sizeof(double) == 8, "Double is not 8 bytes!");
static_assert(sizeof(float)  == 4, "Float is not 4 bytes!");


int main(int argc, const char* argv[])
{
	if(argc != 2)
		lerror(kE_Fatal, nullptr, "Usage: sik path");

	File* theFile = lexFile(argv[1]);

	for(Token* tok : theFile->tokenVector)
	{
		printf("%s\n", tok->mStr.c_str());
	}

	Parser theParser(theFile);
	LambdaBody* theBody = theParser.parse();

	printErrors();
	delete theFile;
	delete theBody;
	return 0;
}