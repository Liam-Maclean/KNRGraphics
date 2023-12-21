#include <iostream>
#include "CompilerDriver.h"


int main(int argc, char* argv[])
{
	BisonShaderCompilerDriver* lexBison = new BisonShaderCompilerDriver(std::cin);

	

	lexBison->yylex();

	delete lexBison;
	return 0;

}