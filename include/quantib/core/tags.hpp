#pragma once
#define NAME static constexpr std::string_view name =

struct RequestTag {
	static constexpr std::string_view name = "";
};
struct SubscriptionTag {
	static constexpr std::string_view name = "";
};

struct AccountIdTag : RequestTag { NAME "AccountIdTag"; };
struct ConnectTag : RequestTag { NAME "ConnectTag"; };
struct NextValidIdTag : RequestTag { NAME "RequestTag"; };
struct AccountSummaryTag : SubscriptionTag { NAME "accountSummaryTag"; };

struct ContractDetailsTag : RequestTag { NAME "ContractDetailsTag"; };
struct OpenOrdersTag : RequestTag { NAME "OpenOrdersTag"; };
struct ExecutionDetailsTag : RequestTag { NAME "ExecutionDetailsTag"; };
struct PositionTag : SubscriptionTag { NAME "PositionTag"; };