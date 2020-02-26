#pragma once
#include <max6675.h>
#include "GlobalDefinitions.h"
#include "DisplayableValue.h"
#include "Loggable.h"

#define EXHAUST_TEMP_NAME F("exhaust")

class ExhaustTemp :
	public DisplayableValue,
	public Loggable,
	private MAX6675
{
	float temp_ = 0.0;
public:
	ExhaustTemp() :
		MAX6675(SPI_SCK_PIN, SPI_CS_PIN, SPI_MISO_PIN)
	{};
	~ExhaustTemp() {};

	String getDisplayableValue() override {
		return String(temp_);
	}
	String getLogName() override {
		return EXHAUST_TEMP_NAME;
	}
	float getLogValue() override {
		return temp_;
	};

	void measure() {
		temp_ = MAX6675::readCelsius();
	}
};