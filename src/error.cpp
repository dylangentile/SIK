#include "error.h"
#include <string>
#include <cstdint>
#include <cstdlib>

std::string error_str = "";
uint32_t error_count = 0;

std::string warning_str = "";
uint32_t warning_count = 0;


void
lerror(Error_Level level, Token* tok, const char* message)
{
	std::string& dest = level == kE_Warning ? warning_str : error_str;
	if(level == kE_Warning) {warning_count++;} else {error_count++;}

	if(tok == nullptr)
	{
		dest += std::string(message);
	}
	else
	{

		dest += std::string(tok->parentFile->path) + ":" + std::to_string(tok->lineNumber) + ": "
			 +	std::string(message) + "\n\t";

		const char* ogPtr = tok->parentFile->content.c_str();

		for(const char* ptr = ogPtr + tok->offset; ptr != ogPtr + tok->offset + 60 && *ptr != '\n' && *ptr != '\0'; ptr++)
			dest.push_back(*ptr);

		dest += "\n\t^";

		for(uint32_t i = 1; i < tok->count; i++) 
			dest.push_back('~');
	}

	dest += "\n\n";

	if(level == kE_Fatal)
	{
		printErrors();
		exit(1);
	}

}


void
printErrors()
{
	printf("%s\n%s\n", warning_str.c_str(), error_str.c_str());

	printf("sik exited with %u errors and %u warnings.\n", error_count, warning_count);
}

