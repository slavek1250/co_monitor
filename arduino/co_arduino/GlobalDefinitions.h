#pragma once
#ifndef GLOBAL_DEFINITIONS_
#define GLOBAL_DEFINITIONS_

#include <Arduino.h>

#define BAUDRATE 115200

#define SSID	F("ssid")
#define PASS	F("pass")
#define API_URL F("http://example.com/api")


#define MAX_WAIT_FOR_HANDSHAKE	30
#define MAX_WAIT_FOR_CONF		60

#define MAX_PAGES_NUM 5

#define DALLAS_NUM			4
#define DS_FEEDER_TEMP		0	//ds1
#define DS_MAIN_TEMP		1	//ds2
#define DS_ADDITIONAL_TEMP	2	//ds3
#define DS_OUTDOOR_TEMP		3	//ds4

// PINS
#define RX_PIN				0
#define TX_PIN				1
#define ESP_RESET_PIN		2
#define ONE_WIRE_PIN		3
#define LCD_D0_PIN			4
#define LCD_D1_PIN			5
#define LCD_D2_PIN			6
#define LCD_D3_PIN			7
#define LCD_RS_PIN			8
#define LCD_ENABLE_PIN		9
#define LCD_HIGHLIGHT_PIN	10
#define SPI_MISO_PIN		11
#define SPI_CS_PIN			12
#define SPI_SCK_PIN			13
#define BUTTONS_PIN			A0
#define LIMITING_SWITCH_PIN A1
#define SIGNAL_DETECTOR_PIN A2
#define RELAY_PIN			A3
#define SR04_TRIG_PIN		A4
#define SR04_ECHO_PIN		A5

#define RELAY_ON				HIGH
#define RELAY_OFF				LOW
#define SIG_DET_ACTIVE			HIGH
#define LIMITTING_SWITCH_ACTIVE LOW
#define LCD_HIGHLIGHTED			HIGH
#define LCD_DIM					LOW
#define ESP_RESET				LOW

#define WDT_MAX_TIME				1000
#define BUTTONS_SCAN_PERIOD			5
#define LCD_REFRESH_PERIOD			500
#define RESETING_IN_MIN_MOD_PERIOD	60000
#define DATA_SENDING_PERIOD			60000
#define MEASURING_PERIOD			1000
#define ESP_RESET_PERIOD			100
#define MAX_TIME_NOT_ANS_ESP		300000UL

#define SIG_DET_ONE_ROT_MAX_TIME	19000

#define SR04_MIN_DISTANCE	2
#define SR04_MAX_DISTANCE	200

#endif