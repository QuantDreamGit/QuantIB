"""
Copyright (C) 2026 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable.
"""

from ibapi.object_implem import Object
from ibapi.tag_value import TagValue
from ibapi.order import Order


class AvailableAlgoParams(Object):

    # ! [scale_params]
    @staticmethod
    def FillScaleParams(baseOrder: Order, scaleInitLevelSize: int, scaleSubsLevelSize: int, scaleRandomPercent: bool,
                        scalePriceIncrement: float, scalePriceAdjustValue: float, scalePriceAdjustInterval: int,
                        scaleProfitOffset: float, scaleAutoReset: bool, scaleInitPosition: int, scaleInitFillQty: int):
        baseOrder.scaleInitLevelSize = scaleInitLevelSize # Initial Component Size
        baseOrder.scaleSubsLevelSize = scaleSubsLevelSize # Subsequent Comp. Size
        baseOrder.scaleRandomPercent = scaleRandomPercent # Randomize size by +/-55%
        baseOrder.scalePriceIncrement = scalePriceIncrement # Price Increment

        # Auto Price adjustment
        baseOrder.scalePriceAdjustValue = scalePriceAdjustValue # starting price by
        baseOrder.scalePriceAdjustInterval = scalePriceAdjustInterval # in seconds

        # Profit Orders
        baseOrder.scaleProfitOffset = scaleProfitOffset # Create profit taking order Profit Offset
        baseOrder.scaleAutoReset = scaleAutoReset # Restore size after taking profit
        baseOrder.scaleInitPosition = scaleInitPosition # Initial Position
        baseOrder.scaleInitFillQty = scaleInitFillQty # Filled initial Component Size
    # ! [scale_params]

    # ! [arrivalpx_params]
    @staticmethod
    def FillArrivalPriceParams(baseOrder: Order, maxPctVol: float,
                               riskAversion: str, startTime: str, endTime: str,
                               forceCompletion: bool, allowPastTime: bool):
        baseOrder.algoStrategy = "ArrivalPx"
        baseOrder.algoParams = []
        baseOrder.algoParams.append(TagValue("maxPctVol", maxPctVol))
        baseOrder.algoParams.append(TagValue("riskAversion", riskAversion))
        baseOrder.algoParams.append(TagValue("startTime", startTime))
        baseOrder.algoParams.append(TagValue("endTime", endTime))
        baseOrder.algoParams.append(TagValue("forceCompletion",
                                             int(forceCompletion)))
        baseOrder.algoParams.append(TagValue("allowPastEndTime",
                                             int(allowPastTime)))


    # ! [arrivalpx_params]


    # ! [darkice_params]
    @staticmethod
    def FillDarkIceParams(baseOrder: Order, displaySize: int, startTime: str,
                          endTime: str, allowPastEndTime: bool):
        baseOrder.algoStrategy = "DarkIce"
        baseOrder.algoParams = []
        baseOrder.algoParams.append(TagValue("displaySize", displaySize))
        baseOrder.algoParams.append(TagValue("startTime", startTime))
        baseOrder.algoParams.append(TagValue("endTime", endTime))
        baseOrder.algoParams.append(TagValue("allowPastEndTime",
                                             int(allowPastEndTime)))


    # ! [darkice_params]


    # ! [pctvol_params]
    @staticmethod
    def FillPctVolParams(baseOrder: Order, pctVol: float, startTime: str,
                         endTime: str, noTakeLiq: bool):
        baseOrder.algoStrategy = "PctVol"
        baseOrder.algoParams = []
        baseOrder.algoParams.append(TagValue("pctVol", pctVol))
        baseOrder.algoParams.append(TagValue("startTime", startTime))
        baseOrder.algoParams.append(TagValue("endTime", endTime))
        baseOrder.algoParams.append(TagValue("noTakeLiq", int(noTakeLiq)))


    # ! [pctvol_params]


    # ! [twap_params]
    @staticmethod
    def FillTwapParams(baseOrder: Order, startTime: str,
                       endTime: str, allowPastEndTime: bool):
        baseOrder.algoStrategy = "Twap"
        baseOrder.algoParams = []
        baseOrder.algoParams.append(TagValue("startTime", startTime))
        baseOrder.algoParams.append(TagValue("endTime", endTime))
        baseOrder.algoParams.append(TagValue("allowPastEndTime",
                                             int(allowPastEndTime)))

    # ! [twap_params]


    # ! [vwap_params]
    @staticmethod
    def FillVwapParams(baseOrder: Order, maxPctVol: float, startTime: str,
                       endTime: str, allowPastEndTime: bool, noTakeLiq: bool,
                       speedUp: bool):
        baseOrder.algoStrategy = "Vwap"
        baseOrder.algoParams = []
        baseOrder.algoParams.append(TagValue("maxPctVol", maxPctVol))
        baseOrder.algoParams.append(TagValue("startTime", startTime))
        baseOrder.algoParams.append(TagValue("endTime", endTime))
        baseOrder.algoParams.append(TagValue("allowPastEndTime",
                                             int(allowPastEndTime)))
        baseOrder.algoParams.append(TagValue("noTakeLiq", int(noTakeLiq)))
        baseOrder.algoParams.append(TagValue("speedUp", int(speedUp)))


    # ! [vwap_params]


    # ! [ad_params]
    @staticmethod
    def FillAccumulateDistributeParams(baseOrder: Order, componentSize: int,
                                       timeBetweenOrders: int, randomizeTime20: bool, randomizeSize55: bool,
                                       giveUp: int, catchUp: bool, waitForFill: bool, startTime: str,
                                       endTime: str):
        baseOrder.algoStrategy = "AD"
        baseOrder.algoParams = []
        baseOrder.algoParams.append(TagValue("componentSize", componentSize))
        baseOrder.algoParams.append(TagValue("timeBetweenOrders", timeBetweenOrders))
        baseOrder.algoParams.append(TagValue("randomizeTime20",
                                             int(randomizeTime20)))
        baseOrder.algoParams.append(TagValue("randomizeSize55",
                                             int(randomizeSize55)))
        baseOrder.algoParams.append(TagValue("giveUp", giveUp))
        baseOrder.algoParams.append(TagValue("catchUp", int(catchUp)))
        baseOrder.algoParams.append(TagValue("waitForFill", int(waitForFill)))
        baseOrder.algoParams.append(TagValue("activeTimeStart", startTime))
        baseOrder.algoParams.append(TagValue("activeTimeEnd", endTime))

    # ! [ad_params]


    # ! [balanceimpactrisk_params]
    @staticmethod
    def FillBalanceImpactRiskParams(baseOrder: Order, maxPctVol: float,
                                    riskAversion: str, forceCompletion: bool):
        baseOrder.algoStrategy = "BalanceImpactRisk"
        baseOrder.algoParams = []
        baseOrder.algoParams.append(TagValue("maxPctVol", maxPctVol))
        baseOrder.algoParams.append(TagValue("riskAversion", riskAversion))
        baseOrder.algoParams.append(TagValue("forceCompletion",
                                             int(forceCompletion)))

    # ! [balanceimpactrisk_params]


    # ! [minimpact_params]
    @staticmethod
    def FillMinImpactParams(baseOrder: Order, maxPctVol: float):
        baseOrder.algoStrategy = "MinImpact"
        baseOrder.algoParams = []
        baseOrder.algoParams.append(TagValue("maxPctVol", maxPctVol))

    # ! [minimpact_params]


    # ! [accudistr_params]
    @staticmethod
    def FillAccuDistrParams(baseOrder: Order, timeBetweenOrders: int,
                            routeOrderType: str, componentSize: int,
                            activeTimeStart: str, activeTimeEnd: str,
                            activeTimeTz: str):
        baseOrder.algoStrategy = "AccuDistr"
        baseOrder.algoParams = []
        baseOrder.algoParams.append(TagValue("timeBetweenOrders", timeBetweenOrders))
        baseOrder.algoParams.append(TagValue("routeOrderType", routeOrderType))
        baseOrder.algoParams.append(TagValue("componentSize", componentSize))
        baseOrder.algoParams.append(TagValue("activeTimeStart", activeTimeStart))
        baseOrder.algoParams.append(TagValue("activeTimeEnd", activeTimeEnd))
        baseOrder.algoParams.append(TagValue("activeTimeTz", activeTimeTz))

    # ! [accudistr_params]


    # ! [adaptive_params]
    @staticmethod
    def FillAdaptiveParams(baseOrder: Order, priority: str):
        baseOrder.algoStrategy = "Adaptive"
        baseOrder.algoParams = []
        baseOrder.algoParams.append(TagValue("adaptivePriority", priority))

    # ! [adaptive_params]

    # ! [closepx_params]
    @staticmethod
    def FillClosePriceParams(baseOrder: Order, maxPctVol: float, riskAversion: str,
                             startTime: str, forceCompletion: bool):
        baseOrder.algoStrategy = "ClosePx"
        baseOrder.algoParams = []
        baseOrder.algoParams.append(TagValue("maxPctVol", maxPctVol))
        baseOrder.algoParams.append(TagValue("riskAversion", riskAversion))
        baseOrder.algoParams.append(TagValue("startTime", startTime))
        baseOrder.algoParams.append(TagValue("forceCompletion", int(forceCompletion)))


    # ! [closepx_params]


    # ! [pctvolpx_params]
    @staticmethod
    def FillPriceVariantPctVolParams(baseOrder: Order, pctVol: float,
                                     deltaPctVol: float, minPctVol4Px: float,
                                     maxPctVol4Px: float, startTime: str,
                                     endTime: str, noTakeLiq: bool):
        baseOrder.algoStrategy = "PctVolPx"
        baseOrder.algoParams = []
        baseOrder.algoParams.append(TagValue("pctVol", pctVol))
        baseOrder.algoParams.append(TagValue("deltaPctVol", deltaPctVol))
        baseOrder.algoParams.append(TagValue("minPctVol4Px", minPctVol4Px))
        baseOrder.algoParams.append(TagValue("maxPctVol4Px", maxPctVol4Px))
        baseOrder.algoParams.append(TagValue("startTime", startTime))
        baseOrder.algoParams.append(TagValue("endTime", endTime))
        baseOrder.algoParams.append(TagValue("noTakeLiq", int(noTakeLiq)))


    # ! [pctvolpx_params]


    # ! [pctvolsz_params]
    @staticmethod
    def FillSizeVariantPctVolParams(baseOrder: Order, startPctVol: float,
                                    endPctVol: float, startTime: str,
                                    endTime: str, noTakeLiq: bool):
        baseOrder.algoStrategy = "PctVolSz"
        baseOrder.algoParams = []
        baseOrder.algoParams.append(TagValue("startPctVol", startPctVol))
        baseOrder.algoParams.append(TagValue("endPctVol", endPctVol))
        baseOrder.algoParams.append(TagValue("startTime", startTime))
        baseOrder.algoParams.append(TagValue("endTime", endTime))
        baseOrder.algoParams.append(TagValue("noTakeLiq", int(noTakeLiq)))


    # ! [pctvolsz_params]


    # ! [pctvoltm_params]
    @staticmethod
    def FillTimeVariantPctVolParams(baseOrder: Order, startPctVol: float,
                                    endPctVol: float, startTime: str,
                                    endTime: str, noTakeLiq: bool):
        baseOrder.algoStrategy = "PctVolTm"
        baseOrder.algoParams = []
        baseOrder.algoParams.append(TagValue("startPctVol", startPctVol))
        baseOrder.algoParams.append(TagValue("endPctVol", endPctVol))
        baseOrder.algoParams.append(TagValue("startTime", startTime))
        baseOrder.algoParams.append(TagValue("endTime", endTime))
        baseOrder.algoParams.append(TagValue("noTakeLiq", int(noTakeLiq)))

    # ! [pctvoltm_params]

def Test():
    av = AvailableAlgoParams() # @UnusedVariable


if "__main__" == __name__:
    Test()
