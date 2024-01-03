#include <iostream>
#include "CompilerDriver.h"
#define yyFlexLexer ShaderCompilerLexer
#include <FlexLexer.h>

struct ShaderCompileArgs
{
	bool stripPDBs = false;
	bool stripReflection = false;
	bool generatePDBs = false;
	int optimisationLevel = 0;
	char* shaderFile = "";
	char* entryPoint = "";
	char* outputExtension = "";
	char* shaderModel = "";
};

int main(int argc, char* argv[])
{
	ShaderCompilerLexer* lexBison = new ShaderCompilerLexer(std::cin, std::cerr);

	lexBison->yylex();

	delete lexBison;
	return 0;

}