#pragma once
#include <string>

#include "EWrapper.h"
#include "Contract.h"
#include "EClientSocket.h"
#include "object_hub.hpp"
#include "quantib/network/connection.hpp"
#include "quantib/utils/logger.hpp"

struct GreeksData {
	double implied_volatility;
	double delta;
	double price;
	double pv_dividend;
	double gamma;
	double vega;
	double theta;
	double und_price;

	explicit GreeksData(const double implied_volatility_, const double delta_, const double price_,
						const double pv_dividend_, const double gamma_, const double vega_, const double theta_,
						const double und_price) : implied_volatility(implied_volatility_), delta(delta_), price(price_),
												  pv_dividend(pv_dividend_), gamma(gamma_), vega(vega_), theta(theta_),
												  und_price(und_price) {}
	};

	struct MarketData {
		std::unordered_map<TickType, double> generic_map;
		std::unordered_map<TickType, double> price_map;
		std::unordered_map<TickType, Decimal> size_map;
		std::unordered_map<TickType, std::string> string_map;
		std::unordered_map<TickType, GreeksData> option_map;

		bool has_price() const {
			if (const auto price = getPrice(); price.has_value()) return true;
			return false;
		}

		std::optional<double> getPrice() const {
			if (price_map.empty()) return std::nullopt;

			// Priority: real-time trade, then quote, then delayed trade/quote, then mark/close/open.
			static constexpr TickType kPriority[] = {
				TickType::LAST, TickType::BID, TickType::ASK, TickType::DELAYED_LAST, TickType::DELAYED_BID,
				TickType::DELAYED_ASK, TickType::MARK_PRICE, TickType::CLOSE, TickType::DELAYED_CLOSE, TickType::OPEN,
				TickType::DELAYED_OPEN
			};

			for (TickType t : kPriority) {
				auto it = price_map.find(t);
				if (it != price_map.end()) { return it->second; }
			}

			return -1.0;
		}
	};

	struct MarketDataStore {
		mutable std::shared_mutex mtx;
		MarketData data;
	};

	class DataManager {
	public:
		DataManager(EClientSocket& client, BlockingHub& hub, ObjectHub& obj, Logger& logger,
		            const int market_data_type = 3) : logger_(logger), client_(client), obj_(obj), hub_(hub) {
			setMarketDataType(market_data_type);
			data_map_ = &obj_.create<MarketDataStoreTag, std::unordered_map<int, MarketDataStore>>();
		}

		void setMarketDataType(const int market_data_type = 3) const {
			/* Market data types:
			 * - 1:	Live market data is streaming data relayed back in real time. Market data subscriptions are required
			 *		to receive live market data.
			 * - 2: Frozen market data is the last data recorded at market close. In TWS, Frozen data is displayed in gray numbers.
			 *		When you set the market data type to Frozen, you are asking TWS to send the last available quote when there is
			 *		not one currently available. For instance, if a market is currently closed and real time data is requested,
			 *		-1 values will commonly be returned for the bid and ask prices to indicate there is no current bid/ask data
			 *		available. TWS will often show a ‘frozen’ bid/ask which represents the last value recorded by the system.
			 *		To receive the last know bid/ask price before the market close, switch to market data type 2 from the API
			 *		before requesting market data. API frozen data requires TWS/IBG v.962 or higher and the same market data
			 *		subscriptions necessary for real time streaming data.
			 * - 3: Free, delayed data is 15 – 20 minutes delayed. In TWS, delayed data is displayed in brown background.
			 *		When you set market data type to delayed, you are telling TWS to automatically switch to delayed market data
			 *		if the user does not have the necessary real time data subscription. If live data is available a request for
			 *		delayed data would be ignored by TWS. Delayed market data is returned with delayed Tick Types (Tick ID 66~76).
			 * - 4: Requests delayed “frozen” data for a user without market data subscriptions.
			 */
			client_.reqMarketDataType(market_data_type);
		}

		int getMarketDataType() const { return *obj_.try_get<MarketDataTypeTag, int>(); }

		void marketDataSub(const Contract& contract, const std::string& tick_list = "", bool snap = false,
		                   bool reg_snap = false) {
			conId_to_id_map_[contract.conId] = getNextId();
			client_.reqMktData(getCurrentId(), contract, tick_list, snap, reg_snap, TagValueListSPtr());
			subscribed_conId_.insert(getCurrentId());
		}

		[[nodiscard]] MarketData getMarketData(const Contract& contract) {
			return getMarketData(conId_to_id_map_[contract.conId]);
		}

		[[nodiscard]] MarketData getMarketData(const int req_id) const {
			// For data safety it's better to get a copy
			const auto it = data_map_->find(req_id);
			if (it == data_map_->end()) return {};

			return it->second.data;
		}

		[[nodiscard]] bool hasPrice(const Contract& contract) {
			if (const MarketData res = getMarketData(contract); res.has_price()) return true;
			return false;
		}

		[[nodiscard]] std::optional<double> getPrice(const Contract& contract) {
			const MarketData res = getMarketData(contract);
			return res.getPrice();
		}

		[[nodiscard]] double getAsk(const Contract& contract);
		[[nodiscard]] double getBid(const Contract& contract);
		[[nodiscard]] double getLast(const Contract& contract);

		[[nodiscard]] bool hasSubscribed(const Contract& contract) {
			if (subscribed_conId_.contains(conId_to_id_map_[contract.conId])) return true;
			return false;
		}

		[[nodiscard]] int getNextId() const { return obj_.get_increment_int<NextIdTag>(); }
		[[nodiscard]] int getCurrentId() const { return *obj_.try_get<NextIdTag, int>(); }

	private:
		Logger& logger_;
		EClientSocket& client_;
		ObjectHub& obj_;
		BlockingHub& hub_;

		std::unordered_map<int, MarketDataStore>* data_map_;
		std::unordered_map<int, int> conId_to_id_map_;
		std::unordered_set<int> subscribed_conId_;
	};
