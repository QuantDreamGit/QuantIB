/* Copyright (C) 2026 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

package samples.testbed.marketdata;

import com.ib.client.protobuf.CancelMarketDataProto;
import com.ib.client.protobuf.ContractProto;
import com.ib.client.protobuf.MarketDataRequestProto;

public class MarketDataSamplesProto {

    public static MarketDataRequestProto.MarketDataRequest OddLotBidAskQuotesRequest(int reqId, ContractProto.Contract contract) {
        //! [odd_lot_bid_ask_quotes_request]
        MarketDataRequestProto.MarketDataRequest.Builder marketDataRequestBuilder = MarketDataRequestProto.MarketDataRequest.newBuilder();
        marketDataRequestBuilder.setReqId(reqId);
        marketDataRequestBuilder.setContract(contract);
        marketDataRequestBuilder.setGenericTickList("787");
        //! [odd_lot_bid_ask_quotes_request]
        return marketDataRequestBuilder.build();
    }

    public static MarketDataRequestProto.MarketDataRequest RegulatorySnapshotRequest(int reqId, ContractProto.Contract contract) {
        //! [regulatory_snapshot_request]
        MarketDataRequestProto.MarketDataRequest.Builder marketDataRequestBuilder = MarketDataRequestProto.MarketDataRequest.newBuilder();
        marketDataRequestBuilder.setReqId(reqId);
        marketDataRequestBuilder.setContract(contract);
        marketDataRequestBuilder.setRegulatorySnapshot(true);
        //! [regulatory_snapshot_request]
        return marketDataRequestBuilder.build();
    }

    public static CancelMarketDataProto.CancelMarketData CancelMarketDataRequest(int reqId) {
        //! [cancel_market_data_request]
        CancelMarketDataProto.CancelMarketData.Builder cancelMarketDataBuilder = CancelMarketDataProto.CancelMarketData.newBuilder();
        cancelMarketDataBuilder.setReqId(reqId);
        //! [cancel_market_data_request]
        return cancelMarketDataBuilder.build();
    }
}
