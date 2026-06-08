#pragma once

#define NAME static constexpr std::string_view name =

struct RequestTag {
	static constexpr std::string_view name = "";
};
struct SubscriptionTag {
	static constexpr std::string_view name = "";
};

struct ConnectTag : RequestTag { NAME "ConnectTag"; };
struct NextValidIdTag : RequestTag { NAME "RequestTag"; };

struct AccountSummaryTag : SubscriptionTag { NAME "accountSummaryTag"; };
