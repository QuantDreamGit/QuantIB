#pragma once
#include <string>
#include <ctime>
#include <fstream>
#include <iostream>
#include <utility>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

// Log level macros
#define LOG_TRACE_LVL spdlog::level::trace
#define LOG_DEBUG_LVL spdlog::level::debug
#define LOG_INFO_LVL spdlog::level::info
#define LOG_ERROR_LVL spdlog::level::err
#define LOG_CRITICAL_LVL spdlog::level::critical
// Variadic macros so that it's more handy to log
#define LOG_INFO_TAG(tag, msg, ...) logger_->info("{}" msg, tag, ##__VA_ARGS__)
#define LOG_DEBUG_TAG(tag, msg, ...) logger_->debug("{}" msg, tag, ##__VA_ARGS__)
#define LOG_ERROR_TAG(tag, msg, ...) logger_->error("{}" msg, tag, ##__ VA_ARGS__)
#define LOG_WARN_TAG(tag, msg, ...) logger_->warn("{}" msg, tag, ##__VA_ARGS__)
#define LOG_CRITICAL_TAG(tag, msg, ...) logger_->critical("{}" msg, tag, ##__VA_ARGS__)
#define LOG_TRACE_TAG(tag, msg, ...) logger_->trace("{}" msg, tag, ##__VA_ARGS__)
// Macros to locate the source of the message
#define LOGGER "[Logger] "
#define OBJ_HUB "[ObjectHub] "
#define CONN_HUB "[ConnectionHub] "
#define IB_STR "[IB] "
#define STRATEGY "[Strategy] "
#define WRAPPER "[Wrapper] "
#define CLIENT "[Client] "
#define REQ_ID "[RequestId] "
#define PERFTIMER "[PerfTimer] "

class Logger {
	// I'd like to design it using 2 modes, silent and verbose with different degrees of freedom.
	// This will be the scale: STRATEGY, INFO, DEBUG, ERROR
public:
	using log_level = spdlog::level::level_enum;
	explicit Logger(const log_level level = LOG_DEBUG_LVL, std::string path = "", const bool verbose = true,
	                const bool write = true) : logger_({}), level_(level), verbose_(verbose),
	                                           write_(write),
	                                           path_(std::move(path)) {
		// Get current date
		const time_t timestamp = time(nullptr); // Time from unix epoch
		const tm datetime = *localtime(&timestamp); // Human-Readable format
		char date[20];
		strftime(date, 20, "%y_%m_%d_%H%M%S", &datetime);

		try {
			// Console logger
			auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			console_sink->set_level(level);
			console_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
			// File logger
			auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path_ + date + "_log.txt", true);
			file_sink->set_level(level);
			file_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
			// Finally, create the multi-sink logger
			logger_ = std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list{console_sink, file_sink});
			logger_->set_level(level);
		} catch (const spdlog::spdlog_ex &ex) {
			std::cerr << "Log init failed: " << ex.what() << std::endl;
		}

		LOG_DEBUG_TAG(LOGGER, "Initialized correctly with level: {}.", spdlog::level::to_string_view(level_));
	}

	template<typename... Args>
	void info(fmt::format_string<Args...> msg, Args&&... args) {
		logger_->info(msg, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void debug(fmt::format_string<Args...> msg, Args&&... args) {
		logger_->debug(msg, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void error(fmt::format_string<Args...> msg, Args&&... args) {
		logger_->error(msg, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void critical(fmt::format_string<Args...> msg, Args&&... args) {
		logger_->critical(msg, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void trace(fmt::format_string<Args...> msg, Args&&... args) {
		logger_->trace(msg, std::forward<Args>(args)...);
	}

	template<typename... Args>
	void warn(fmt::format_string<Args...> msg, Args&&... args) {
		logger_->warn(msg, std::forward<Args>(args)...);
	}

private:
	std::shared_ptr<spdlog::logger> logger_;
	spdlog::level::level_enum level_;
	bool verbose_ = true;
	bool write_ = true;
	std::string path_;
};
