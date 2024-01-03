#pragma once
#include "logger/logger.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

namespace KNR
{
	namespace LightweightFileSystem
	{
		template<typename T>
		static std::vector<T> LoadFile(const char* path);

		template<typename T>
		static void WriteFile(const char* path, std::vector<T> data);
	}
}

template<typename T>
std::vector<T> KNR::LightweightFileSystem::LoadFile(const char* path)
{
		std::ifstream file;
		file.open(path, std::ios::in | std::ios::binary);
		if (!file.is_open())
		{
			KNT_ERROR("failed to open file!");
			assert(0);
		}

		//Read everything out using a string stream and grab the string for it 
		std::string buffer;
		std::stringstream stringStreamBuffer;
		stringStreamBuffer << file.rdbuf();
		buffer = stringStreamBuffer.str();

		//Seek to the end of the string stream and tell me what size this file is 
		stringStreamBuffer.seekg(0, std::ios::end);
		int size = stringStreamBuffer.tellg();

		std::vector<T> fileData;
		fileData.resize(size / sizeof(T));
		memcpy(fileData.data(), buffer.data(), size);

		return fileData;
}

template <typename T>
void KNR::LightweightFileSystem::WriteFile(const char* relativePath, std::vector<T> dataToFile)
{
	std::filesystem::path path(relativePath);
	if (!std::filesystem::exists(path.parent_path()))
		std::filesystem::create_directories(path.parent_path());

	std::ofstream file;
	file.open(path.string(), std::ios::out | std::ios::trunc | std::ios::binary);
	if (!file.is_open())
	{
		KNT_ERROR("failed to open file!");
		assert(0);
	}
	file.write(reinterpret_cast<char*>(dataToFile.data()), dataToFile.size() * sizeof(T));
	file.close();
}