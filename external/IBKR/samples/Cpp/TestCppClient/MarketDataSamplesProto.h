/* Copyright (C) 2026 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */
#pragma once
#ifndef TWS_API_SAMPLES_TESTCPPCLIENT_MARKETDATASAMPLESPROTO_H
#define TWS_API_SAMPLES_TESTCPPCLIENT_MARKETDATASAMPLESPROTO_H

#include "MarketDataRequest.pb.h"
#include "CancelMarketData.pb.h"
#include "Contract.pb.h"

class MarketDataSamplesProto {
public:
#if !defined(USE_WIN_DLL)
    static protobuf::MarketDataRequest OddLotBidAskQuotesRequest(int reqId, const protobuf::Contract& contract);
    static protobuf::MarketDataRequest RegulatorySnapshotRequest(int reqId, const protobuf::Contract& contract);
    static protobuf::CancelMarketData CancelMarketDataRequest(int reqId);
#endif
};

#endif
