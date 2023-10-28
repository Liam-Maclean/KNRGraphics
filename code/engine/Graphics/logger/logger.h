#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace KNR
{
	class Logger
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetConsoleLogger() { return s_console_logger; }
	private:
		static std::shared_ptr<spdlog::logger> s_console_logger;
	};
}

#if 1
#define KNT_TRACE(...) ::KNR::Logger::GetConsoleLogger()->trace(__VA_ARGS__);

#define KNT_ERROR(...) ::KNR::Logger::GetConsoleLogger()->error(__VA_ARGS__); 

#define KNT_WARN(...) ::KNR::Logger::GetConsoleLogger()->warn(__VA_ARGS__);

#define KNT_DEBUG(...) ::KNR::Logger::GetConsoleLogger()->debug(__VA_ARGS__);

#define KNT_INFO(...)  ::KNR::Logger::GetConsoleLogger()->info(__VA_ARGS__);

#else

#define KNT_TRACE
#define KNT_ERROR
#define KNT_WARN
#define KNT_DEBUG
#define KNT_INFO

#endif