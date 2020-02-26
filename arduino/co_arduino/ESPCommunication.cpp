#include "ESPCommunication.h"



void ESPCommunication::processLine(String & line){
	SimpleJSON json(line);
	
	lastDataMs = millis();

	String type = json.getString(F("handshake"));
	if (type != NULL_STR) {
		if (type == F("ESP")) gotHandshake_ = true;
		return;
	}

	type = json.getString(F("type"));
	
	if (type != NULL_STR) {
		if (type == F("CONF")) {
			loadConfig(json);
			gotConf_ = true;
		}
		else if (type == F("ACK")) {
			lastDataTransferGotAck = true;
		}
		else if (type == F("NACK")) {
			lastDataTransferGotNack = true;
		}
	}
}

void ESPCommunication::loadConfig(SimpleJSON & conf) {
	
	for (uint8_t i = 0; i < ds18b20Number; ++i) {
		ds18b20[i]->setAddress(
			conf.getString(ds18b20[i]->getName())
		);
	}

	safetyParams->setParametersId(
		conf.getInt(F("id"))
	);
	safetyParams->setFeederMaxTemperture(
		conf.getInt(F("fmt"))
	);
	safetyParams->setFeederMaxDistance(
		conf.getInt(F("fmd"))
	);
}

void ESPCommunication::run() {
	if (Serial.available()) {
		char c = (char)Serial.read();
		if (c != END_LINE_CHAR) {
			buff += c;
		}
		else {
			processLine(buff);
			buff = "";
		}
	}
}

void ESPCommunication::addLoggable(Loggable * lg) {
	Loggable ** lgTmp = new Loggable*[loggableNum + 1];
	uint8_t i = 0;
	for (; i < loggableNum; ++i) {
		lgTmp[i] = loggable[i];
	}
	lgTmp[i] = lg;
	if (loggableNum != 0) {
		delete[]loggable;
	}
	loggable = lgTmp;
	++loggableNum;
}

void ESPCommunication::addDallasTermometerToConf(DallasTherm * ds) {

	DallasTherm ** dsTmp = new DallasTherm*[ds18b20Number + 1];
	uint8_t i = 0;
	for (; i < ds18b20Number; ++i) {
		dsTmp[i] = ds18b20[i];
	}
	dsTmp[i] = ds;
	if (ds18b20Number != 0) {
		delete[]ds18b20;
	}
	ds18b20 = dsTmp;
	++ds18b20Number;
}

void ESPCommunication::setSafetyParametersToConf(SafetyParams * sp) {
	safetyParams = sp;
}

void ESPCommunication::sendHandshake() {
	buff = F("{\"handshake\":\"ARDUINO\",\"ssid\":\"");
	buff += SSID;
	buff += F("\",\"pass\":\"");
	buff += PASS;
	buff += F("\",\"url\":\"");
	buff += API_URL;
	buff += F("\"}");
	Serial.println(buff);
	buff = "";
}

bool ESPCommunication::gotHandshake() {
	return gotHandshake_;
}

bool ESPCommunication::gotConfiguration() {
	return gotConf_;
}

void ESPCommunication::sendData() {
	buff = F("{\"id\":");
	buff += String(safetyParams->getParametersId());
	buff += F(",\"type\":\"DATA\",");

	for(uint8_t i = 0; i < loggableNum; ++i) {
		buff += F("\"");
		buff += loggable[i]->getLogName();
		buff += F("\":");
		buff += String(loggable[i]->getLogValue()); 
		buff += F(",");
	}
	buff[buff.length() - 1] = '}';

	Serial.println(buff);
	buff = "";
}