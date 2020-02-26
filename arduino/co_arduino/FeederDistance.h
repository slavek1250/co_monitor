#pragma once
#include "GlobalDefinitions.h"
#include "DisplayableValue.h"
#include "Loggable.h"

#define FEEDER_DISTANCE_NAME F("distance")

class FeederDistance :
	public DisplayableValue,
	public Loggable
{
	float dist_ = 0.0;
	float sum_ = 0.0;
	uint8_t N_ = 0;

public:
	FeederDistance() {};
	~FeederDistance() {};

	String getDisplayableValue() override {
		return String(dist_);
	}
	String getLogName() override {
		return FEEDER_DISTANCE_NAME;
	}
	float getLogValue() override {
		dist_ = N_ ? sum_ / N_ : 0.0;
		sum_ = 0.0;
		N_ = 0;
		return dist_;
	};

	void clear() {
		dist_ = 0.0;
		N_ = 0;
	}

	void init() {
		pinMode(SR04_TRIG_PIN, OUTPUT);
		digitalWrite(SR04_TRIG_PIN, LOW);
		pinMode(SR04_ECHO_PIN, INPUT);
	}

	void measure();
};

void FeederDistance::measure() {

	digitalWrite(SR04_TRIG_PIN, LOW);
	delayMicroseconds(2);
	digitalWrite(SR04_TRIG_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(SR04_TRIG_PIN, LOW);
	
	uint32_t time = pulseIn(SR04_ECHO_PIN, HIGH, 25000);
	if (time != 0) {

		dist_ = time * 34 / 2000.0;
		if (dist_ > SR04_MAX_DISTANCE || dist_ < SR04_MIN_DISTANCE) {
			dist_ = 0;
			return;
		}
		sum_ += dist_;
		++N_;
	}
}