#pragma once
#include "DisplayButtons.h"
#include "GlobalDefinitions.h"
#include "DisplayableValue.h"
#include "Runnable.h"


class FeederDriver :
	public DisplayableValue,
	public Runnable
{
	uint8_t rotationNum = 1;
	uint8_t rotationCurrNum = 0;
	uint8_t limitationSwitchState = DisplayButtons::KeyStates::WAIT_FOR_PRESS;

	DallasTherm * thermometer = nullptr;
	SafetySingleParam<uint16_t> * feederMaxTemp = nullptr;

	bool working_ = false;

	void turnOnFeeder() {
		digitalWrite(RELAY_PIN, RELAY_ON);
		working_ = true;
	}
	void turnOffFeeder() {
		digitalWrite(RELAY_PIN, RELAY_OFF);
		working_ = false;
	}

	void limitationSwitchHandler();
	void signalDetector();

public:
	FeederDriver() {};
	~FeederDriver() {};

	void run() override;

	bool isWorking() { return working_; }
	void init() {
		pinMode(LIMITING_SWITCH_PIN, INPUT_PULLUP);
		pinMode(SIGNAL_DETECTOR_PIN, INPUT);
		pinMode(RELAY_PIN, OUTPUT);
		digitalWrite(RELAY_PIN, RELAY_OFF);
	}

	String getDisplayableValue() override {
		return (String(rotationCurrNum) + "/" + String(rotationNum));
	}
	void setDallasThermometer(DallasTherm * ds) {
		thermometer = ds;
	}
	void setFeederMaxTempInstance(SafetySingleParam<uint16_t> * sp) {
		feederMaxTemp = sp;
	}
};

inline void FeederDriver::limitationSwitchHandler()
{
	if (millis() % 50 == 0) {
		using key = DisplayButtons::KeyStates;

		if (limitationSwitchState == key::WAIT_FOR_PRESS &&
			digitalRead(LIMITING_SWITCH_PIN) == LIMITTING_SWITCH_ACTIVE) {
			limitationSwitchState = key::PRESSED;
			return;
		}
		if (limitationSwitchState == key::PRESSED) {
			limitationSwitchState =
				(digitalRead(LIMITING_SWITCH_PIN) == LIMITTING_SWITCH_ACTIVE) ?
				key::TO_HANDLE : key::WAIT_FOR_PRESS;
		}
		if (limitationSwitchState == key::TO_HANDLE) {
			++rotationCurrNum;
			limitationSwitchState = key::WAIT_FOR_RELEASE_1;
		}
		if (limitationSwitchState == key::WAIT_FOR_RELEASE_1 &&
			digitalRead(LIMITING_SWITCH_PIN) != LIMITTING_SWITCH_ACTIVE) {
			limitationSwitchState = key::WAIT_FOR_RELEASE_2;
			return;
		}
		if (limitationSwitchState == key::WAIT_FOR_RELEASE_2 &&
			digitalRead(LIMITING_SWITCH_PIN) != LIMITTING_SWITCH_ACTIVE) {
			limitationSwitchState = key::WAIT_FOR_PRESS;
		}
	}
}

inline void FeederDriver::signalDetector()
{
	static uint32_t signalStart = 0;
	if (digitalRead(SIGNAL_DETECTOR_PIN) == SIG_DET_ACTIVE) {
		if (signalStart == 0) {
			signalStart = millis();
			turnOnFeeder();
			rotationCurrNum = 1;
		}
	}
	else if (signalStart != 0) {
		if ((millis() - signalStart) > SIG_DET_ONE_ROT_MAX_TIME) {
			rotationNum = 2;
		}
		else {
			rotationNum = 1;
		}
		signalStart = 0;
	}
}

inline void FeederDriver::run() {

	signalDetector();
	limitationSwitchHandler();
	
	if (thermometer != nullptr && feederMaxTemp != nullptr) {
		if (thermometer->getLogValue() > (float)feederMaxTemp->getParam() && 
			(uint8_t)thermometer->getLogValue() != 85) 
		{
			if(!working_)
				turnOnFeeder();
			return;
		}
	}

	if (rotationCurrNum > rotationNum && working_) {
		turnOffFeeder();
		rotationCurrNum = 0;
	}
}