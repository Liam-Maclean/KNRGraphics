#pragma once
#include "core/core.h"

namespace QRender
{
	class ShaderCache
	{
	public:
		void Init(const std::string& sourcePath);

		std::string ParseFileAndIncludes(const std::string& sourcePath);

		bool ShaderNeedsCompiled();

		inline const std::string& GetShaderCode() { return m_code; }

		inline const std::string& GetSourcePath() { return m_sourcePath; }

		inline const std::string& GetAssembly() { return m_assembly; }


		void WriteToFile(std::vector<uint32_t> shaderBytecode);
		std::vector<uint32_t> ReadFromFile();
	private:
		std::string m_sourcePath;
		std::string m_code;
		std::string m_tempFilePath;
		std::string m_assembly;
		std::string m_preprocessed;
		std::vector<std::string> m_includes;
	};
}