#include <iostream>
#include "CompilerDriver.h"
#define yyFlexLexer ShaderCompilerLexer
#include <FlexLexer.h>

extern bool bisonArgStripPDBs;
extern bool bisonArgStripReflection;
extern bool bisonArgGeneratePDBs;
extern int bisonArgOptimisationLevel;
extern char* bisonArgShaderFile;
extern char* bisonArgOutputExtension;
extern char* bisonArgShaderModel;

int main(int argc, char* argv[])
{
	ShaderCompilerLexer* lexBison = new ShaderCompilerLexer(std::cin, std::cerr);

	lexBison->yylex();

	delete lexBison;
	return 0;

}