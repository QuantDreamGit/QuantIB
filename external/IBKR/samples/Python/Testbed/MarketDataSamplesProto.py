"""
Copyright (C) 2026 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable.
"""

from ibapi.protobuf.MarketDataRequest_pb2 import MarketDataRequest as MarketDataRequestProto
from ibapi.protobuf.CancelMarketData_pb2 import CancelMarketData as CancelMarketDataProto
from ibapi.protobuf.Contract_pb2 import Contract as ContractProto

class MarketDataSamplesProto:

    @staticmethod
    def OddLotBidAskQuotesRequest(reqId: int, contract: ContractProto) -> MarketDataRequestProto:
        #! [odd_lot_bid_ask_quotes_request]
        marketDataRequestProto = MarketDataRequestProto()
        marketDataRequestProto.reqId = reqId
        marketDataRequestProto.contract.CopyFrom(contract)
        marketDataRequestProto.genericTickList = "787"
        #! [odd_lot_bid_ask_quotes_request]
        return marketDataRequestProto

    @staticmethod
    def RegulatorySnapshotRequest(reqId: int, contract: ContractProto) -> MarketDataRequestProto:
        #! [regulatory_snapshot_request]
        marketDataRequestProto = MarketDataRequestProto()
        marketDataRequestProto.reqId = reqId
        marketDataRequestProto.contract.CopyFrom(contract)
        marketDataRequestProto.regulatorySnapshot = True
        #! [regulatory_snapshot_request]
        return marketDataRequestProto

    @staticmethod
    def CancelMarketDataRequest(reqId: int) -> CancelMarketDataProto:
        #! [cancel_market_data_request]
        cancelMarketDataProto = CancelMarketDataProto()
        cancelMarketDataProto.reqId = reqId
        #! [cancel_market_data_request]
        return cancelMarketDataProto

def Test():
    from ibapi.utils import ExerciseStaticMethods
    ExerciseStaticMethods(MarketDataSamplesProto)

if "__main__" == __name__:
    Test()
