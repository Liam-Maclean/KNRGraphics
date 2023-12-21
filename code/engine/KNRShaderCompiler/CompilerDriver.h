#include <string>
#include <FlexLexer.h>
class BisonShaderCompilerDriver : public yyFlexLexer
{
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
public:
	BisonShaderCompilerDriver(std::istream& fileName);
	~BisonShaderCompilerDriver();

	int yylex();

private:
	int ParseFile(std::string fileName);
	

	std::string fileName;
};