#pragma once
#include <set>
#include <string>

struct OptionChain {
	using exp_strike_pair = std::pair<std::vector<std::string>, std::vector<double>>;

	int req_id;
	int underlying_con_id;
	std::string trading_class;
	std::string multiplier;
	std::string exchange;
	std::set<std::string> expirations;
	std::set<double> strikes;

	OptionChain(const int req_id_, const int underlying_con_id_, const std::string& trading_class_,
	            const std::string& multiplier_, const std::string& exchange_, const std::set<std::string>& expirations_,
	            const std::set<double>& strikes_)
		: req_id(req_id_), underlying_con_id(underlying_con_id_), trading_class(trading_class_),
		  multiplier(multiplier_), exchange(exchange_), expirations(expirations_), strikes(strikes_) {}

	exp_strike_pair getFromNearestDate(int nExps, int nStrikes, double und_price) {
		std::vector<std::string> out_exp;
		std::vector<double> out_strikes;

		for (auto &exp : expirations) {
			out_exp.push_back(exp);
			if (out_exp.size() >= nExps) break;
		}

		auto center = strikes.lower_bound(und_price);
		auto left = center;
		for (int i = 0; i < nStrikes && left != strikes.begin(); ++i) {
			--left;
		}

		auto right = center;
		for (int i = 0; i < nStrikes && right != strikes.end(); ++i) {
			++right;
		}

		for (auto it = left; it != right; ++it) {
			out_strikes.push_back(*it);
		}

		return std::pair(out_exp, out_strikes);
	}

	static std::optional<OptionChain> getExchangeFromOptionChain(const std::vector<OptionChain>& option_chains, const std::string& exchange) {
		for (const auto& chain : option_chains) {
			if (chain.exchange == exchange) {
				return chain;
			}
		}
		return std::nullopt;
	}
};