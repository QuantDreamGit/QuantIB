#include "quantib/core/contract_manager.h"

#include <chrono>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

std::optional<bool> ContractInfo::isInLiquidHours() const {
	if (pending_details) return std::nullopt;

	// liquid_hours example:
	// 20260619:CLOSED;20260620:CLOSED;20260621:CLOSED;20260622:0930-20260622:1600;20260623:0930-20260623:1600;20260624:0930-20260624:1600
	const std::string& liquid_hours = contract_details.liquidHours;
	// time_xone example: US/Eastern
	std::string time_zone = contract_details.timeZoneId;
	if (time_zone == "MET") time_zone = "Europe/Paris";

	if (liquid_hours.empty() || time_zone.empty()) return std::nullopt;

	// Get local zone, time, local time
	const auto* zone = std::chrono::locate_zone(time_zone);
	const auto now_sys = std::chrono::system_clock::now();
	const auto now_local = std::chrono::zoned_time{zone, now_sys}.get_local_time();
	// Parse datetime
	const auto today_days = std::chrono::floor<std::chrono::days>(now_local);
	const auto ymd = std::chrono::year_month_day{today_days};
	const int yyyy = int(ymd.year());
	const unsigned mm = unsigned(ymd.month());
	const unsigned dd = unsigned(ymd.day());
	// today_buf example: 20260619
	char today_buf[9];
	std::snprintf(today_buf, sizeof(today_buf), "%04d%02u%02u", yyyy, mm, dd);
	const std::string today = today_buf;

	// Then iterate over each date
	std::stringstream ss(liquid_hours);
	std::string segment;
	while (std::getline(ss, segment, ';')) {
		// Example:
		// 20090507:0700-1830
		if (segment.size() < 15) continue; // 20090507:CLOSED

		const std::string date = segment.substr(0, 8);
		if (date != today) continue;

		const std::string hours = segment.substr(9);
		if (hours == "CLOSED") return false;

		// There may be multiple sessions separated by commas.
		std::stringstream sessions(hours);
		std::string session;

		while (std::getline(sessions, session, ',')) {
			const auto dash = session.find('-');
			if (dash == std::string::npos) continue;

			std::string start_str = session.substr(0, dash);
			std::string end_str = session.substr(dash + 1);

			// Handles: 20260619:0900
			if (start_str.size() == 13 && start_str[8] == ':') start_str = start_str.substr(9);

			// Handles: 20260619:1745
			if (end_str.size() == 13 && end_str[8] == ':') end_str = end_str.substr(9);

			// Now both should be HHMM
			if (start_str.size() != 4 || end_str.size() != 4) continue;

			// Conversion
			const int start_h = std::stoi(start_str.substr(0, 2));
			const int start_m = std::stoi(start_str.substr(2, 2));
			const int end_h = std::stoi(end_str.substr(0, 2));
			const int end_m = std::stoi(end_str.substr(2, 2));
			// Compute start/end hour
			const auto start = today_days + std::chrono::hours{start_h} + std::chrono::minutes{start_m};

			const auto end = today_days + std::chrono::hours{end_h} + std::chrono::minutes{end_m};
			// Verify
			if (now_local >= start && now_local < end) return true;
		}

		return false;
	}

	// No matching date found in liquidHours.
	return std::nullopt;
}
