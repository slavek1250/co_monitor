#include <DS18B20.h>
#include <OneWire.h>
#include <leOS2.h>
#include <LiquidCrystal.h>

#include "GlobalDefinitions.h"
#include "Display.h"
#include "DisplayButtons.h"
#include "DallasTherm.h"
#include "SafetyParams.h"
#include "ESPCommunication.h"
#include "FeederDriver.h"
#include "ExhaustTemp.h"
#include "FeederDistance.h"

leOS2 myOS;
Display<MAX_PAGES_NUM> lcd(
	LiquidCrystal(LCD_RS_PIN, LCD_ENABLE_PIN, LCD_D0_PIN, LCD_D1_PIN, LCD_D2_PIN, LCD_D3_PIN)
);
DisplayButtons displayButtons;
ESPCommunication esp;
FeederDriver feederDriver;
FeederDistance feederDistance;
ExhaustTemp exhaustTemp;
OneWire oneWire(ONE_WIRE_PIN);
DS18B20 dsHandler(&oneWire);
DallasTherm dsTherms[DALLAS_NUM] = {
	DallasTherm("ds1"),
	DallasTherm("ds2"),
	DallasTherm("ds3"),
	DallasTherm("ds4")
};
SafetyParams safetyParams;

volatile bool extendedModeSetupDone = false;
volatile bool minimalModeTryToReset = true;

void scanButtons();
void switchToMinimalMode();
void extendedModeSetup();
void measure();

void reset() {
	if (!feederDriver.isWorking())
		myOS.reset();
}

void refreshDisplay() {
	lcd.run();
}

void sendData() {
	if ((millis() - esp.getLastDataMillis()) > MAX_TIME_NOT_ANS_ESP) {
		reset();
	}
	esp.sendData();
}

void startResetingEsp() {
	pinMode(ESP_RESET_PIN, OUTPUT);
	digitalWrite(ESP_RESET_PIN, LOW);
}

void stopResetingEsp() {
	pinMode(ESP_RESET_PIN, INPUT_PULLUP);
}

void setup() {

	feederDriver.init();

	Serial.begin(BAUDRATE);
	lcd.clear();

	for (uint8_t i = 0; i < DALLAS_NUM; ++i) {
		esp.addDallasTermometerToConf(&dsTherms[i]);
		esp.addLoggable(&dsTherms[i]);
	}
	esp.setSafetyParametersToConf(&safetyParams);
	esp.addLoggable(&exhaustTemp);
	esp.addLoggable(&feederDistance);

	startResetingEsp();
	delay(ESP_RESET_PERIOD);
	stopResetingEsp();
	delay(ESP_RESET_PERIOD);

	myOS.begin(myOS.convertMs(WDT_MAX_TIME));
	myOS.addTask(
		extendedModeSetup,
		myOS.convertMs(100),
		SCHEDULED
	);
	myOS.addTask(
		scanButtons,
		myOS.convertMs(BUTTONS_SCAN_PERIOD),
		SCHEDULED
	);
	myOS.addTask(
		refreshDisplay,
		myOS.convertMs(LCD_REFRESH_PERIOD),
		SCHEDULED
	);

	lcd.addPage(DisplayPage(
		DisplayLine(F("    C.O. sys")),
		DisplayLine(F("Conf..."))
	));	
	lcd.showFirstPage();
}

void loop() {
	esp.run();
	feederDriver.run();
}

void scanButtons() {
	displayButtons.run();

	if (displayButtons.canAnyKeyBeHandled()) {
		if (displayButtons.getKeyCode() == DisplayButtons::KeyCodes::UP) {
			lcd.showPreviousPage();
		}
		if (displayButtons.getKeyCode() == DisplayButtons::KeyCodes::DOWN) {
			lcd.showNextPage();
		}
		if (displayButtons.getKeyCode() == DisplayButtons::KeyCodes::RIGHT) {
			lcd.showFirstPage();
		}
		if (displayButtons.getKeyCode() == DisplayButtons::KeyCodes::LEFT) {
			if (lcd.isHighlighted()) {
				lcd.displayDim();
			}
			else {
				lcd.displayHighlighted();
			}
		}
		if (displayButtons.getKeyCode() == DisplayButtons::KeyCodes::SELECT) {
			minimalModeTryToReset = false;
			if (!extendedModeSetupDone) switchToMinimalMode();
		}
		lcd.run();
		displayButtons.setKeyHandled();
	}
}

void switchToMinimalMode() {
	bool alreadySet = false;
	if (alreadySet) return;

	myOS.pauseTask(extendedModeSetup);
	if (minimalModeTryToReset) {
		myOS.addTask(
			reset,
			myOS.convertMs(RESETING_IN_MIN_MOD_PERIOD),
			SCHEDULED
		);
	}
	lcd.replacePage(
		0,
		DisplayPage(
			DisplayLine(F("    C.O. sys")),
			DisplayLine(F("Min mode    "), &feederDriver)
		)
	);
	alreadySet = true;
}


void extendedModeSetup() {

	static bool handshakeSent = false;
	if (handshakeSent == false) {
		esp.sendHandshake();
		handshakeSent = true;
	}

	static uint8_t waitingForHandshake = 0;
	if (!esp.gotHandshake()) {
		if (waitingForHandshake++ >= MAX_WAIT_FOR_HANDSHAKE) {
			switchToMinimalMode();
		}
	}

	static uint8_t waitingForConf = 0;
	if (!esp.gotConfiguration()) {
		if (waitingForConf++ >= MAX_WAIT_FOR_CONF) {
			switchToMinimalMode();
		}
	}

	if (esp.gotConfiguration() && esp.gotHandshake()) {
		myOS.pauseTask(extendedModeSetup);

		myOS.addTask(
			sendData,
			myOS.convertMs(DATA_SENDING_PERIOD),
			SCHEDULED
		);
		myOS.addTask(
			measure,
			myOS.convertMs(MEASURING_PERIOD),
			SCHEDULED
		);

		dsHandler.begin();
		dsHandler.request();

		lcd.replacePage(
			0,
			DisplayPage(
				DisplayLine(F("    C.O. sys")),
				DisplayLine(F("Ext mode    "), &feederDriver)
			)
		);

		lcd.addPage(
			DisplayPage(
				DisplayLine(F("Podajnik"), &dsTherms[DS_FEEDER_TEMP], F("?C"), DisplaySpecialChars::DEGREE),
				DisplayLine(F("Max     "), &safetyParams.getFeederMaxTempInstance(), F("?C"), DisplaySpecialChars::DEGREE)
			)
		);

		lcd.addPage(
			DisplayPage(
				DisplayLine(F("Paliwo"), &feederDistance, F("cm")),
				DisplayLine(F("Max   "), &safetyParams.getFeederMaxDistInstance(), F("cm"))
			)
		);

		lcd.addPage(
			DisplayPage(
				DisplayLine(F("Piec   "), &dsTherms[DS_MAIN_TEMP], F("?C"), DisplaySpecialChars::DEGREE),
				DisplayLine(F("Spaliny"), &exhaustTemp, F("?C"), DisplaySpecialChars::DEGREE)
			)
		);

		lcd.addPage(
			DisplayPage(
				DisplayLine(F("Zewnatrz"), &dsTherms[DS_OUTDOOR_TEMP], F("?C"), DisplaySpecialChars::DEGREE),
				DisplayLine(F("Inny    "), &dsTherms[DS_ADDITIONAL_TEMP], F("?C"), DisplaySpecialChars::DEGREE)
			)
		);

		feederDriver.setFeederMaxTempInstance(&safetyParams.getFeederMaxTempInstance());
		feederDriver.setDallasThermometer(&dsTherms[DS_FEEDER_TEMP]);
		extendedModeSetupDone = true;
	}
}

void measure() {
	feederDistance.measure();
	exhaustTemp.measure();

	if (dsHandler.available()) {
		for (uint8_t i = 0; i < DALLAS_NUM; ++i) {
			dsTherms[i].setTemperature(
				dsHandler.readTemperature(
					dsTherms[i].getAddress()
				)
			);
		}
	}

	dsHandler.request();
}