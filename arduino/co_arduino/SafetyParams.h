#pragma once
#include <Arduino.h>
#include "DisplayableValue.h"

template <class T>
class SafetySingleParam :
	public DisplayableValue
{
	T value_;

public:
	void setParam(T val) { value_ = val; }
	T getParam() { return value_; }
	String getDisplayableValue() override { return String(value_); }
};

class SafetyParams 
{
	uint32_t paramsId;

	SafetySingleParam<uint16_t> feederMaxTemp;
	SafetySingleParam<uint16_t> feederMaxDistance;
	
public:
	
	SafetyParams() {};
	~SafetyParams() {};

	void setFeederMaxTemperture(uint16_t maxTemp) { feederMaxTemp.setParam(maxTemp); }
	void setFeederMaxDistance(uint16_t maxDist) { feederMaxDistance.setParam(maxDist); }
	void setParametersId(uint32_t id) { paramsId = id; }
	
	SafetySingleParam<uint16_t> & getFeederMaxTempInstance() { return feederMaxTemp; }
	SafetySingleParam<uint16_t> & getFeederMaxDistInstance() { return feederMaxDistance; }

	uint32_t getParametersId() { return paramsId; }

};
