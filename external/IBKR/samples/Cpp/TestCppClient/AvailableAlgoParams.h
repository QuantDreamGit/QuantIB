/* Copyright (C) 2026 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#pragma once
#ifndef TWS_API_SAMPLES_TESTCPPCLIENT_AVAILABLEALGOPARAMS_H
#define TWS_API_SAMPLES_TESTCPPCLIENT_AVAILABLEALGOPARAMS_H

#include <string>

struct Order;

class AvailableAlgoParams {
public:
	static void FillScaleParams(Order& baseOrder, int scaleInitLevelSize, int scaleSubsLevelSize, bool scaleRandomPercent,
		double scalePriceIncrement, double scalePriceAdjustValue, int scalePriceAdjustInterval, double scaleProfitOffset,
		bool scaleAutoReset, int scaleInitPosition, int scaleInitFillQty);
	static void FillArrivalPriceParams(Order& baseOrder, double maxPctVol, std::string riskAversion, std::string startTime, std::string endTime,
            bool forceCompletion, bool allowPastTime);
	static void FillDarkIceParams(Order& baseOrder, int displaySize, std::string startTime, std::string endTime, bool allowPastEndTime);
	static void FillPctVolParams(Order& baseOrder, double pctVol, std::string startTime, std::string endTime, bool noTakeLiq);
	static void FillTwapParams(Order& baseOrder, std::string startTime, std::string endTime, bool allowPastEndTime);
	static void FillVwapParams(Order& baseOrder, double maxPctVol, std::string startTime, std::string endTime, bool allowPastEndTime, bool noTakeLiq, bool speedUp);
	static void FillAccumulateDistributeParams(Order& baseOrder, int componentSize, int timeBetweenOrders, bool randomizeTime20, bool randomizeSize55,
            int giveUp, bool catchUp, bool waitForFill, std::string startTime, std::string endTime);
	static void FillBalanceImpactRiskParams(Order& baseOrder, double maxPctVol, std::string riskAversion, bool forceCompletion);
	static void FillMinImpactParams(Order& baseOrder, double maxPctVol);
	static void FillAccuDistrParams(Order& baseOrder, int timeBetweenOrders, std::string routeOrderType, int componentSize,
		std::string activeTimeStart, std::string activeTimeEnd, std::string activeTimeTz);
	static void FillAdaptiveParams(Order& baseOrder, std::string priority);
	static void FillClosePriceParams(Order& baseOrder, double maxPctVol, std::string riskAversion, std::string startTime, bool forceCompletion);
	static void FillPriceVariantPctVolParams(Order& baseOrder, double pctVol, double deltaPctVol, double minPctVol4Px,
			double maxPctVol4Px, std::string startTime, std::string endTime, bool noTakeLiq);
	static void FillSizeVariantPctVolParams(Order& baseOrder, double startPctVol, double endPctVol,
			std::string startTime, std::string endTime, bool noTakeLiq);
	static void FillTimeVariantPctVolParams(Order& baseOrder, double startPctVol, double endPctVol, std::string startTime,
			std::string endTime, bool noTakeLiq);
};

#endif
