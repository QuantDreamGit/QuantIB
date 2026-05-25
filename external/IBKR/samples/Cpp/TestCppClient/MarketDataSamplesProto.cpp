/* Copyright (C) 2026 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#include "StdAfx.h"

#include "MarketDataSamplesProto.h"

#if !defined(USE_WIN_DLL)

protobuf::MarketDataRequest MarketDataSamplesProto::OddLotBidAskQuotesRequest(int reqId, const protobuf::Contract& contract){
    //! [odd_lot_bid_ask_quotes_request]
    protobuf::MarketDataRequest marketDataRequestProto;
    marketDataRequestProto.set_reqid(reqId);
    *marketDataRequestProto.mutable_contract() = contract;
    marketDataRequestProto.set_genericticklist("787");
    //! [odd_lot_bid_ask_quotes_request]
    return marketDataRequestProto;
}

protobuf::MarketDataRequest MarketDataSamplesProto::RegulatorySnapshotRequest(int reqId, const protobuf::Contract& contract){
    //! [regulatory_snapshot_request]
    protobuf::MarketDataRequest marketDataRequestProto;
    marketDataRequestProto.set_reqid(reqId);
    *marketDataRequestProto.mutable_contract() = contract;
    marketDataRequestProto.set_regulatorysnapshot(true);
    //! [regulatory_snapshot_request]
    return marketDataRequestProto;
}

protobuf::CancelMarketData MarketDataSamplesProto::CancelMarketDataRequest(int reqId){
    //! [cancel_market_data_request]
    protobuf::CancelMarketData cancelMarketDataProto;
    cancelMarketDataProto.set_reqid(reqId);
    //! [cancel_market_data_request]
    return cancelMarketDataProto;
}

#endif
