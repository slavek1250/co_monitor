#pragma once
#include <Arduino.h>
#include "Loggable.h"
#include "DisplayableValue.h"

#define DALLAS_UNIT F("?C")

class DallasTherm :
	public Loggable,
	public DisplayableValue
{
	float temp_ = 0.0;
	const String name;
	uint8_t address[8];

	uint8_t getDecValOfCharHex(char c);

public:
	DallasTherm(const String & name) : name(name) {};
	~DallasTherm() {};

	String getName() { return name; }
	uint8_t * getAddress() { return address; }
	String getUnit() { return DALLAS_UNIT; }

	void setAddress(const String & address);
	void setAddress(uint8_t * addr);

	String getLogName() override { return name; }
	float getLogValue() override { return temp_; }

	String getDisplayableValue() override {
		return String(temp_);
	}

	void setTemperature(float temp) { temp_ = temp; }
};

inline uint8_t DallasTherm::getDecValOfCharHex(char c)
{
	if (c >= 'a' && c <= 'f') {
		return (uint8_t)c - 'a' + 10;
	}
	if (c >= 'A' && c <= 'F') {
		return (uint8_t)c - 'A' + 10;
	}
	if (c >= '0' && c <= '9') {
		return (uint8_t)c - '0';
	}
	return 0;
}

inline void DallasTherm::setAddress(const String & address)
{
	if (address.length() == 16) {
		for (uint8_t i = 0; i < 16; i += 2) {
			uint8_t octetOfAnAddress = getDecValOfCharHex(address[i]);
			octetOfAnAddress = octetOfAnAddress << 4;
			octetOfAnAddress |= getDecValOfCharHex(address[i + 1]);

			this->address[i / 2] = octetOfAnAddress;
		}
	}
}

inline void DallasTherm::setAddress(uint8_t * addr)
{
	for (uint8_t i = 0; i < 8; ++i)
		address[i] = addr[i];
}
