#include "logger.h"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> KNR::Logger::s_console_logger;
std::shared_ptr<spdlog::logger> KNR::Logger::s_file_logger;

namespace KNR
{
	void Logger::Init()
	{
		AllocConsole();

		// colour-timestamp-logger_name-message
		spdlog::set_pattern("%^[%T] %n: %v%$");

		// init console logger
		s_console_logger = spdlog::stdout_color_mt("[KNT]");
		s_console_logger->set_level(spdlog::level::trace);
		s_console_logger->set_pattern("%^[%T] %n: %v%$");


		s_file_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>("knt_log.txt", true);
		s_file_logger->set_level(spdlog::level::trace);
		s_file_logger->set_pattern("%^[%T] %n: %v%$");
	}
}
