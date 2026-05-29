/* Copyright (C) 2026 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */
#include "StdAfx.h"

#include "AvailableAlgoParams.h"
#include "Order.h"

//! [scale_params]
void AvailableAlgoParams::FillScaleParams(Order& baseOrder, int scaleInitLevelSize, int scaleSubsLevelSize, bool scaleRandomPercent,
	double scalePriceIncrement, double scalePriceAdjustValue, int scalePriceAdjustInterval, double scaleProfitOffset,
	bool scaleAutoReset, int scaleInitPosition, int scaleInitFillQty){
	baseOrder.scaleInitLevelSize = scaleInitLevelSize;
	baseOrder.scaleSubsLevelSize = scaleSubsLevelSize;
	baseOrder.scaleRandomPercent = scaleRandomPercent;
	baseOrder.scalePriceIncrement = scalePriceIncrement;
	baseOrder.scalePriceAdjustValue = scalePriceAdjustValue;
	baseOrder.scalePriceAdjustInterval = scalePriceAdjustInterval;
	baseOrder.scaleProfitOffset = scaleProfitOffset;
	baseOrder.scaleAutoReset = scaleAutoReset;
	baseOrder.scaleInitPosition = scaleInitPosition;
	baseOrder.scaleInitFillQty = scaleInitFillQty;
}
//! [scale_params]

//! [arrivalpx_params]
void AvailableAlgoParams::FillArrivalPriceParams(Order& baseOrder, double maxPctVol, std::string riskAversion, std::string startTime, std::string endTime,
	bool forceCompletion, bool allowPastTime){
	baseOrder.algoStrategy = "ArrivalPx";
	baseOrder.algoParams.reset(new TagValueList());
	TagValueSPtr tag1(new TagValue("maxPctVol", std::to_string(maxPctVol)));
	TagValueSPtr tag2(new TagValue("riskAversion", riskAversion));
	TagValueSPtr tag3(new TagValue("startTime", startTime));
	TagValueSPtr tag4(new TagValue("endTime", endTime));
	TagValueSPtr tag5(new TagValue("forceCompletion", forceCompletion ? "1" : "0"));
	TagValueSPtr tag6(new TagValue("allowPastEndTime", allowPastTime ? "1" : "0"));
	baseOrder.algoParams->push_back(tag1);
	baseOrder.algoParams->push_back(tag2);
	baseOrder.algoParams->push_back(tag3);
	baseOrder.algoParams->push_back(tag4);
	baseOrder.algoParams->push_back(tag5);
	baseOrder.algoParams->push_back(tag6);
}
//! [arrivalpx_params]

//! [darkice_params]
void AvailableAlgoParams::FillDarkIceParams(Order& baseOrder, int displaySize, std::string startTime, std::string endTime, bool allowPastEndTime){
	baseOrder.algoStrategy = "DarkIce";
	baseOrder.algoParams.reset(new TagValueList());
	TagValueSPtr tag1(new TagValue("displaySize", std::to_string(displaySize)));
	TagValueSPtr tag2(new TagValue("startTime", startTime));
	TagValueSPtr tag3(new TagValue("endTime", endTime));
	TagValueSPtr tag4(new TagValue("allowPastEndTime", allowPastEndTime ? "1" : "0"));
	baseOrder.algoParams->push_back(tag1);
	baseOrder.algoParams->push_back(tag2);
	baseOrder.algoParams->push_back(tag3);
	baseOrder.algoParams->push_back(tag4);
}
//! [darkice_params]

//! [pctvol_params]
void AvailableAlgoParams::FillPctVolParams(Order& baseOrder, double pctVol, std::string startTime, std::string endTime, bool noTakeLiq){
	baseOrder.algoStrategy = "PctVol";
	baseOrder.algoParams.reset(new TagValueList());
	TagValueSPtr tag1(new TagValue("pctVol", std::to_string(pctVol)));
	TagValueSPtr tag2(new TagValue("startTime", startTime));
	TagValueSPtr tag3(new TagValue("endTime", endTime));
	TagValueSPtr tag4(new TagValue("noTakeLiq", noTakeLiq ? "1" : "0"));
	baseOrder.algoParams->push_back(tag1);
	baseOrder.algoParams->push_back(tag2);
	baseOrder.algoParams->push_back(tag3);
	baseOrder.algoParams->push_back(tag4);
}
//! [pctvol_params]

//! [twap_params]
void AvailableAlgoParams::FillTwapParams(Order& baseOrder, std::string startTime, std::string endTime, bool allowPastEndTime){
	baseOrder.algoStrategy = "Twap";
	baseOrder.algoParams.reset(new TagValueList());
	TagValueSPtr tag1(new TagValue("startTime", startTime));
	TagValueSPtr tag2(new TagValue("endTime", endTime));
	TagValueSPtr tag3(new TagValue("allowPastEndTime", allowPastEndTime ? "1" : "0"));
	baseOrder.algoParams->push_back(tag1);
	baseOrder.algoParams->push_back(tag2);
	baseOrder.algoParams->push_back(tag3);

}
//! [twap_params]

//! [vwap_params]
void AvailableAlgoParams::FillVwapParams(Order& baseOrder, double maxPctVol, std::string startTime, std::string endTime, bool allowPastEndTime, bool noTakeLiq, bool speedUp){
	baseOrder.algoStrategy = "Vwap";
	baseOrder.algoParams.reset(new TagValueList());
	TagValueSPtr tag1(new TagValue("maxPctVol", std::to_string(maxPctVol)));
	TagValueSPtr tag2(new TagValue("startTime", startTime));
	TagValueSPtr tag3(new TagValue("endTime", endTime));
	TagValueSPtr tag4(new TagValue("allowPastEndTime", allowPastEndTime ? "1" : "0"));
	TagValueSPtr tag5(new TagValue("noTakeLiq", noTakeLiq ? "1" : "0"));
	TagValueSPtr tag6(new TagValue("speedUp", speedUp ? "1" : "0"));
	baseOrder.algoParams->push_back(tag1);
	baseOrder.algoParams->push_back(tag2);
	baseOrder.algoParams->push_back(tag3);
	baseOrder.algoParams->push_back(tag4);
	baseOrder.algoParams->push_back(tag5);
	baseOrder.algoParams->push_back(tag6);
}
//! [vwap_params]

//! [ad_params]
void AvailableAlgoParams::FillAccumulateDistributeParams(Order& baseOrder, int componentSize, int timeBetweenOrders, bool randomizeTime20, bool randomizeSize55,
	int giveUp, bool catchUp, bool waitForFill, std::string startTime, std::string endTime){
	baseOrder.algoStrategy = "AD";
	baseOrder.algoParams.reset(new TagValueList());
	TagValueSPtr tag1(new TagValue("componentSize", std::to_string(componentSize)));
	TagValueSPtr tag2(new TagValue("timeBetweenOrders",std::to_string(timeBetweenOrders)));
	TagValueSPtr tag3(new TagValue("randomizeTime20", randomizeTime20 ? "1" : "0"));
	TagValueSPtr tag4(new TagValue("randomizeSize55", randomizeSize55 ? "1" : "0"));
	TagValueSPtr tag5(new TagValue("giveUp", std::to_string(giveUp)));
	TagValueSPtr tag6(new TagValue("catchUp", catchUp ? "1" : "0"));
	TagValueSPtr tag7(new TagValue("waitForFill", waitForFill ? "1" : "0"));
	TagValueSPtr tag8(new TagValue("activeTimeStart", startTime));
	TagValueSPtr tag9(new TagValue("activeTimeEnd", endTime));
	baseOrder.algoParams->push_back(tag1);
	baseOrder.algoParams->push_back(tag2);
	baseOrder.algoParams->push_back(tag3);
	baseOrder.algoParams->push_back(tag4);
	baseOrder.algoParams->push_back(tag5);
	baseOrder.algoParams->push_back(tag6);
	baseOrder.algoParams->push_back(tag7);
	baseOrder.algoParams->push_back(tag8);
	baseOrder.algoParams->push_back(tag9);
}
//! [ad_params]

//! [balanceimpactrisk_params]
void AvailableAlgoParams::FillBalanceImpactRiskParams(Order& baseOrder, double maxPctVol, std::string riskAversion, bool forceCompletion){
	baseOrder.algoStrategy = "BalanceImpactRisk";
	baseOrder.algoParams.reset(new TagValueList());
	TagValueSPtr tag1(new TagValue("maxPctVol", std::to_string(maxPctVol)));
	TagValueSPtr tag2(new TagValue("riskAversion", riskAversion));
	TagValueSPtr tag3(new TagValue("forceCompletion", forceCompletion ? "1" : "0"));
	baseOrder.algoParams->push_back(tag1);
	baseOrder.algoParams->push_back(tag2);
	baseOrder.algoParams->push_back(tag3);
}
//! [balanceimpactrisk_params]

//! [minimpact_params]
void AvailableAlgoParams::FillMinImpactParams(Order& baseOrder, double maxPctVol){
	baseOrder.algoStrategy = "MinImpact";
	baseOrder.algoParams.reset(new TagValueList());
	TagValueSPtr tag1(new TagValue("maxPctVol", std::to_string(maxPctVol)));
	baseOrder.algoParams->push_back(tag1);
}
//! [minimpact_params]

//! [accudistr_params]
void AvailableAlgoParams::FillAccuDistrParams(Order& baseOrder, int timeBetweenOrders, std::string routeOrderType, int componentSize,
	std::string activeTimeStart, std::string activeTimeEnd, std::string activeTimeTz){
	baseOrder.algoStrategy = "AccuDistr";
	baseOrder.algoParams.reset(new TagValueList());
	TagValueSPtr tag1(new TagValue("timeBetweenOrders", std::to_string(timeBetweenOrders)));
	TagValueSPtr tag2(new TagValue("routeOrderType", routeOrderType));
	TagValueSPtr tag3(new TagValue("componentSize", std::to_string(componentSize)));
	TagValueSPtr tag4(new TagValue("activeTimeStart", activeTimeStart));
	TagValueSPtr tag5(new TagValue("activeTimeEnd", activeTimeEnd));
	TagValueSPtr tag6(new TagValue("activeTimeTz", activeTimeTz));
	baseOrder.algoParams->push_back(tag1);
	baseOrder.algoParams->push_back(tag2);
	baseOrder.algoParams->push_back(tag3);
	baseOrder.algoParams->push_back(tag4);
	baseOrder.algoParams->push_back(tag5);
	baseOrder.algoParams->push_back(tag6);
}
//! [accudistr_params]

//! [adaptive_params]
void AvailableAlgoParams::FillAdaptiveParams(Order& baseOrder, std::string priority){
	baseOrder.algoStrategy = "Adaptive";
	baseOrder.algoParams.reset(new TagValueList());
	TagValueSPtr tag1(new TagValue("adaptivePriority", priority));
	baseOrder.algoParams->push_back(tag1);
}
//! [adaptive_params]

//! [closepx_params]
void AvailableAlgoParams::FillClosePriceParams(Order& baseOrder, double maxPctVol, std::string riskAversion, std::string startTime, bool forceCompletion){
	baseOrder.algoStrategy = "ClosePx";
	baseOrder.algoParams.reset(new TagValueList());
	TagValueSPtr tag1(new TagValue("maxPctVol", std::to_string(maxPctVol)));
	TagValueSPtr tag2(new TagValue("riskAversion",riskAversion));
	TagValueSPtr tag3(new TagValue("startTime", startTime));
	TagValueSPtr tag4(new TagValue("forceCompletion", forceCompletion ? "1" : "0"));
	baseOrder.algoParams->push_back(tag1);
	baseOrder.algoParams->push_back(tag2);
	baseOrder.algoParams->push_back(tag3);
	baseOrder.algoParams->push_back(tag4);
}
//! [closepx_params]

//! [pctvolpx_params]
void AvailableAlgoParams::FillPriceVariantPctVolParams(Order& baseOrder, double pctVol, double deltaPctVol, double minPctVol4Px,
													   double maxPctVol4Px, std::string startTime, std::string endTime, bool noTakeLiq){
	baseOrder.algoStrategy = "PctVolPx";
	baseOrder.algoParams.reset(new TagValueList());
	TagValueSPtr tag1(new TagValue("pctVol", std::to_string(pctVol)));
	TagValueSPtr tag2(new TagValue("deltaPctVol", std::to_string(deltaPctVol)));
	TagValueSPtr tag3(new TagValue("minPctVol4Px", std::to_string(minPctVol4Px)));
	TagValueSPtr tag4(new TagValue("maxPctVol4Px", std::to_string(maxPctVol4Px)));
	TagValueSPtr tag5(new TagValue("startTime", startTime));
	TagValueSPtr tag6(new TagValue("endTime", endTime));
	TagValueSPtr tag7(new TagValue("noTakeLiq", noTakeLiq ? "1" : "0"));
	baseOrder.algoParams->push_back(tag1);
	baseOrder.algoParams->push_back(tag2);
	baseOrder.algoParams->push_back(tag3);
	baseOrder.algoParams->push_back(tag4);
	baseOrder.algoParams->push_back(tag5);
	baseOrder.algoParams->push_back(tag6);
	baseOrder.algoParams->push_back(tag7);
}
//! [pctvolpx_params]

//! [pctvolsz_params]
void AvailableAlgoParams::FillSizeVariantPctVolParams(Order& baseOrder, double startPctVol, double endPctVol,
								std::string startTime, std::string endTime, bool noTakeLiq){
	baseOrder.algoStrategy = "PctVolSz";
	baseOrder.algoParams.reset(new TagValueList());
	TagValueSPtr tag1(new TagValue("startPctVol", std::to_string(startPctVol)));
	TagValueSPtr tag2(new TagValue("endPctVol", std::to_string(endPctVol)));
	TagValueSPtr tag3(new TagValue("startTime", startTime));
	TagValueSPtr tag4(new TagValue("endTime", endTime));
	TagValueSPtr tag5(new TagValue("noTakeLiq", noTakeLiq ? "1" : "0"));
	baseOrder.algoParams->push_back(tag1);
	baseOrder.algoParams->push_back(tag2);
	baseOrder.algoParams->push_back(tag3);
	baseOrder.algoParams->push_back(tag4);
	baseOrder.algoParams->push_back(tag5);
}
//! [pctvolsz_params]

//! [pctvoltm_params]
void AvailableAlgoParams::FillTimeVariantPctVolParams(Order& baseOrder, double startPctVol, double endPctVol, std::string startTime,
								 std::string endTime, bool noTakeLiq){
	baseOrder.algoStrategy = "PctVolTm";
	baseOrder.algoParams.reset(new TagValueList());
	TagValueSPtr tag1(new TagValue("startPctVol", std::to_string(startPctVol)));
	TagValueSPtr tag2(new TagValue("endPctVol", std::to_string(endPctVol)));
	TagValueSPtr tag3(new TagValue("startTime", startTime));
	TagValueSPtr tag4(new TagValue("endTime", endTime));
	TagValueSPtr tag5(new TagValue("noTakeLiq", noTakeLiq ? "1" : "0"));
	baseOrder.algoParams->push_back(tag1);
	baseOrder.algoParams->push_back(tag2);
	baseOrder.algoParams->push_back(tag3);
	baseOrder.algoParams->push_back(tag4);
	baseOrder.algoParams->push_back(tag5);
}
//! [pctvoltm_params]