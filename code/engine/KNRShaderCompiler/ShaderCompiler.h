class ShaderCompiler
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
	ShaderCompiler();
	~ShaderCompiler();

private:
	void CompileShader(const ShaderCompileArgs& compilerArgs);
	void ReflectShader();
};