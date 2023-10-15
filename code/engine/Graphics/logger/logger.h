#pragma once
#include "spdlog/spdlog.h"

namespace KNR
{
	class Logger
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetConsoleLogger() { return s_console_logger; }
		inline static std::shared_ptr<spdlog::logger>& GetFileLogger() { return s_file_logger; }
	private:
		static std::shared_ptr<spdlog::logger> s_console_logger;
		static std::shared_ptr<spdlog::logger> s_file_logger;
	};
}

#if 1
#define KNT_TRACE(...) ::KNR::Logger::GetConsoleLogger()->trace(__VA_ARGS__); \
					   ::KNR::Logger::GetFileLogger()->trace(__VA_ARGS__);

#define KNT_ERROR(...) ::KNR::Logger::GetConsoleLogger()->error(__VA_ARGS__); \
					   ::KNR::Logger::GetFileLogger()->error(__VA_ARGS__);

//#define KNT_WARN(...) ::KNR::Logger::GetConsoleLogger()->warn(__VA_ARGS__); \ 
//					  ::KNR::Logger::GetFileLogger()->warn(__VA_ARGS__);

#define KNT_DEBUG(...) ::KNR::Logger::GetConsoleLogger()->debug(__VA_ARGS__); \
					   ::KNR::Logger::GetFileLogger()->debug(__VA_ARGS__);

#define KNT_INFO(...)  ::KNR::Logger::GetConsoleLogger()->info(__VA_ARGS__); \
					   ::KNR::Logger::GetFileLogger()->info(__VA_ARGS__);
#else

#define KNT_TRACE
#define KNT_ERROR
#define KNT_WARN
#define KNT_DEBUG
#define KNT_INFO

#endif