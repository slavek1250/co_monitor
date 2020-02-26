#pragma once
class SafetyPramas
{
	uint32_t paramsId;
	uint16_t feederMaxTemp, feederMaxDistance;

public:
	SafetyPramas() {};
	~SafetyPramas() {};

	void setFeederMaxTemperture(uint16_t maxTemp) { feederMaxTemp = maxTemp; }
	void setFeederMaxDistance(uint16_t maxDist) { feederMaxDistance = maxDist; }
	void setParametersId(uint32_t id) { paramsId = id; }
	
	uint16_t getFeederMaxTemperature() { return feederMaxTemp; }
	uint16_t getFeederMaxDistance() { return feederMaxDistance; }
	uint32_t getParametersId() { return paramsId; }
};