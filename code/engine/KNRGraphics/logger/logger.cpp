#include "logger.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> KNR::Logger::s_console_logger;

namespace KNR
{
	void Logger::Init()
	{
		AllocConsole();

		// colour-timestamp-logger_name-message
		spdlog::set_pattern("%^[%T] %n: %v%$");

		auto console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
		console_sink->set_level(spdlog::level::trace);
		console_sink->set_pattern("%^[%T] %n: %v%$");


		auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>("knt_log.txt", true);
		file_logger->set_level(spdlog::level::trace);
		file_logger->set_pattern("%^[%T] %n: %v%$");

		auto logger = spdlog::logger("[KNT]", { console_sink , file_logger });
		// init console logger
		s_console_logger = std::make_shared<spdlog::logger>(logger);
		s_console_logger->set_level(spdlog::level::trace);
		s_console_logger->set_pattern("%^[%T] %n: %v%$");


	
	}
}
