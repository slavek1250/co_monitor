#pragma once
#include "Runnable.h"
#include "GlobalDefinitions.h"

#define ADC_K_RIGHT 0
#define ADC_K_UP 100
#define ADC_K_DOWN 250
#define ADC_K_LEFT 400
#define ADC_K_SELECT 600
#define ADC_K_NONE 950

class DisplayButtons :
	public Runnable
{
public:
	DisplayButtons() {};
	~DisplayButtons() {};

	enum KeyCodes {
		RIGHT,
		UP,
		DOWN,
		LEFT,
		SELECT,
		NONE
	};
	enum KeyStates {
		WAIT_FOR_PRESS,
		PRESSED,
		TO_HANDLE,
		WAIT_FOR_RELEASE_1,
		WAIT_FOR_RELEASE_2
	};

	void run() override;
	bool canAnyKeyBeHandled();
	uint8_t getKeyCode();
	void setKeyHandled();

private: 
	uint8_t keyState = KeyStates::WAIT_FOR_PRESS;
	uint8_t keyCode = KeyCodes::NONE;
	bool keyHandled_ = false;
};


/** 
 * Should be executed each 5ms.
*/
inline void DisplayButtons::run()
{
	uint8_t newKeyCode;

	uint16_t adcVal = analogRead(BUTTONS_PIN);
	if (adcVal >= ADC_K_NONE) {
		newKeyCode = KeyCodes::NONE;
	}
	else if (adcVal >= ADC_K_SELECT) {
		newKeyCode = KeyCodes::SELECT;
	}
	else if (adcVal >= ADC_K_LEFT) {
		newKeyCode = KeyCodes::LEFT;
	}
	else if (adcVal >= ADC_K_DOWN) {
		newKeyCode = KeyCodes::DOWN;
	}
	else if (adcVal >= ADC_K_UP) {
		newKeyCode = KeyCodes::UP;
	}
	else if (adcVal >= ADC_K_RIGHT) {
		newKeyCode = KeyCodes::RIGHT;
	}

	if (keyState == KeyStates::WAIT_FOR_PRESS && newKeyCode != KeyCodes::NONE) {
		keyState = KeyStates::PRESSED;
		return;
	}
	if (keyState == KeyStates::PRESSED) {

		keyState = newKeyCode != KeyCodes::NONE ? KeyStates::TO_HANDLE : KeyStates::WAIT_FOR_PRESS;

	}
	if (keyState == KeyStates::TO_HANDLE) {
		if (!keyHandled_) {
			keyCode = newKeyCode;
			return;
		}
		else {
			keyState = KeyStates::WAIT_FOR_RELEASE_1;
		}
	}

	if (keyState == KeyStates::WAIT_FOR_RELEASE_1 && newKeyCode == KeyCodes::NONE) {
		keyState = KeyStates::WAIT_FOR_RELEASE_2;
		return;
	}
	if (keyState == KeyStates::WAIT_FOR_RELEASE_2 && newKeyCode == KeyCodes::NONE) {
		keyState = KeyStates::WAIT_FOR_PRESS;
		keyCode = KeyCodes::NONE;
		keyHandled_ = false;
	}
}

inline bool DisplayButtons::canAnyKeyBeHandled()
{
	return keyCode != KeyCodes::NONE && !keyHandled_;
}

inline uint8_t DisplayButtons::getKeyCode()
{
	return keyCode;
}

inline void DisplayButtons::setKeyHandled()
{
	keyHandled_ = true;
}

