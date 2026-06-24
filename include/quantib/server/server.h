#pragma once
#include "httplib.h"

#include "quantib/sync/ib.hpp"

template <typename ProfileT>
class HttpServerIB {
public:
	explicit HttpServerIB(IB<ProfileT>& ib) : ib_(ib) { setupRoutes(); }

	~HttpServerIB() { if (connected) server_.stop(); }

	void start(std::string host = "127.0.0.1", int port = 8080);

private:
	void setupRoutes();

	IB<ProfileT>& ib_;
	httplib::Server server_;
	bool connected = false;
};
