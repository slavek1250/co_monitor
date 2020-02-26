#include <ESP8266WiFi.h>
#include "GlobalDefinitions.h"
#include "ArduinoCommunication.h"
#include "WebApi.h"

ArduinoCommunication arduino;
WebApi webApi;

void connectionMonitor() {
	if (WiFi.status() != WL_CONNECTED) {
		WiFi.begin(arduino.getSSID(), arduino.getPass());
		while (WiFi.waitForConnectResult() != WL_CONNECTED) {
			WiFi.begin(arduino.getSSID(), arduino.getPass());
		}
	}
}

void setup()
{
	Serial.begin(BAUDRATE);
	WiFi.hostname("C_O_telemetry");

	while (!arduino.gotHandshake()) {
		arduino.run();
	}

	connectionMonitor();

	webApi.setUrl(arduino.getApiUrl());
	webApi.fetchConfiguration();
}

void loop()
{
	arduino.run();

	if (arduino.hasDataForApi()) {
		bool successfullySent = webApi.sendData(arduino.getDataForApi());
		if (successfullySent && !webApi.hasGotConfiguration()) {
			arduino.sendAck();
		}
		else if (!successfullySent) {
			arduino.sendNack();
		}
		arduino.deleteDataForApi();
	}

	if (webApi.hasGotConfiguration()) {
		arduino.sendDataFromApi(webApi.getConfiguration());
		webApi.deleteConfiguration();
	}
}
