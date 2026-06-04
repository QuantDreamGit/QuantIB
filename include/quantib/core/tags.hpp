#pragma once

struct RequestTag {};
struct SubscriptionTag {};

struct connectTag : RequestTag {};
struct nextValidIdTag : RequestTag {};

struct accountSummaryTag : SubscriptionTag {};
