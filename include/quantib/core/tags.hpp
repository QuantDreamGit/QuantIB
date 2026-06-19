#pragma once
#include <string_view>

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Request tag		-> automatically deleted after response is received;
// Subscription tag -> automatically deleted after subscription is canceled;
// Permanent tag	-> never deleted, used for storing data that should be available
//					   throughout the program's lifetime;
struct RequestTag {
	static constexpr std::string_view category = "request";
};

struct SubscriptionTag {
	static constexpr std::string_view category = "subscription";
};

struct PermanentTag {
	static constexpr std::string_view category = "permanent";
};
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Permanent objects
struct OrderStoreTag : PermanentTag {
	static constexpr std::string_view name = "OrderStoreTag";
};

struct PositionStoreTag : PermanentTag, SubscriptionTag {
	static constexpr std::string_view name = "PositionStoreTag";
};

struct ContractStoreTag : PermanentTag {
	static constexpr std::string_view name = "ContractStoreTag";
};

struct ContractSymbolStoreTag : PermanentTag {
	static constexpr std::string_view name = "ContractSymbolStoreTag";
};

struct MarketDataStoreTag : PermanentTag {
	static constexpr std::string_view name = "MarketDataStoreTag";
};

struct AccountStoreTag : PermanentTag {
	static constexpr std::string_view name = "AccountStoreTag";
};

struct OpenOrderStoreTag : PermanentTag {
	static constexpr std::string_view name = "OpenOrderStoreTag";
};

struct ClosedOrderStoreTag : PermanentTag {
	static constexpr std::string_view name = "ClosedOrderStoreTag";
};

struct BulletinStoreTag : PermanentTag {
	static constexpr std::string_view name = "BulletinStoreTag";
};

struct NextIdTag : PermanentTag {
	static constexpr std::string_view name = "NextIdTag";
};
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Request objects
struct AccountIdTag : RequestTag {
	static constexpr std::string_view name = "AccountIdTag";
};

struct ContractDetailsTag : RequestTag {
	static constexpr std::string_view name = "ContractDetailsTag";
};

struct ConnectTag : RequestTag {
	static constexpr std::string_view name = "ConnectTag";
};

struct NextValidIdTag : RequestTag {
	static constexpr std::string_view name = "NextValidIdTag";
};

struct OpenOrdersTag : RequestTag {
	static constexpr std::string_view name = "OpenOrdersTag";
};

struct ClosedOrdersTag : RequestTag {
	static constexpr std::string_view name = "ClosedOrdersTag";
};

struct MarketDataTypeTag : RequestTag {
	static constexpr std::string_view name = "MarketDataTypeTag";
};
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Subscription objects
struct AccountSummaryTag : SubscriptionTag {
	static constexpr std::string_view name = "AccountSummaryTag";
};