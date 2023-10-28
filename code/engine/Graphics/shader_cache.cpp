#include "shader_cache.h"
#include <sstream>
#include <filesystem>
#include <fstream>
namespace KNR
{
	void ShaderCache::Init(const std::string& sourcePath)
	{

		std::string shaderCachePath = sourcePath.substr(0, sourcePath.length()); //Clip off the 6 letters plus . 
		//shaderCachePath += "hlsl";


		//m_sourcePath = shaderCachePath;

		//m_code = ParseFileAndIncludes(m_sourcePath);

		m_tempFilePath = shaderCachePath;
	}
	std::string ShaderCache::ParseFileAndIncludes(const std::string& sourcePath)
	{
        std::filesystem::path path(sourcePath);
        std::ifstream file;
        file.open(path.string(), std::ios::in | std::ios::ate | std::ios::binary);

        //read the file out to code string
        std::string code;
        file >> code;
		file.close();
        return code;
	}

	bool ShaderCache::ShaderNeedsCompiled()
	{
		return !std::filesystem::exists(m_tempFilePath);
	}

	std::vector<uint32_t> ShaderCache::ReadFromFile()
	{
		std::ifstream file;
		file.open(m_tempFilePath, std::ios::in | std::ios::binary);

		//Read everything out using a string stream and grab the string for it 
		std::string buffer;
		std::stringstream stringStreamBuffer;
		stringStreamBuffer << file.rdbuf();
		buffer = stringStreamBuffer.str();

		//Seek to the end of the string stream and tell me what size this file is 
		stringStreamBuffer.seekg(0, std::ios::end);
		int size = stringStreamBuffer.tellg();

		std::vector<uint32_t> shaderBytecode;
		shaderBytecode.resize(size / sizeof(uint32_t));
		memcpy(shaderBytecode.data(), buffer.data(), size);
		
		return shaderBytecode;
	}

	void ShaderCache::WriteToFile(std::vector<uint32_t> shaderBytecode)
	{
		std::filesystem::path path(m_tempFilePath);
		if (!std::filesystem::exists(path.parent_path()))
			std::filesystem::create_directories(path.parent_path());

		std::ofstream file;
		file.open(path.string(), std::ios::out | std::ios::trunc | std::ios::binary);

		file.write(reinterpret_cast<char*>(shaderBytecode.data()), shaderBytecode.size() * sizeof(uint32_t));
		file.close();
	}
}