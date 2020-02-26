#pragma once
#include "SimpleJSON.h"

#define END_LINE_CHAR '\n'

class ArduinoCommunication
{
	bool gotHandshake_ = false;
	bool hasDataForApi_ = false;
	String ssid, pass, apiUrl, dataForApi;

	void processLine(String & line);
	void loadParams(SimpleJSON & json);
	void sendHandshake();

public:
	ArduinoCommunication() {};
	~ArduinoCommunication() {};

	bool gotHandshake() { return gotHandshake_; }
	bool hasDataForApi() { return hasDataForApi_; }
	String getSSID() { return ssid; }
	String getPass() { return pass; }
	String getApiUrl() { return apiUrl; }
	String getDataForApi() { return dataForApi; }
	void deleteDataForApi();
	
	void sendAck();
	void sendNack();
	void sendDataFromApi(const String & data);
	void run();
};

inline void ArduinoCommunication::processLine(String & line) {
	SimpleJSON json(line);

	String type = json.getString("handshake");
	if (type != NULL_STR && !gotHandshake_) {
		if (type == "ARDUINO") {
			loadParams(json);
			sendHandshake();
		}
	}

	type = json.getString("type");
	if (type != NULL_STR) {
		if (type == "DATA") {
			dataForApi = line;
			hasDataForApi_ = true;
		}
	}
}

inline void ArduinoCommunication::loadParams(SimpleJSON & json) {
	ssid = json.getString("ssid");
	pass = json.getString("pass");
	apiUrl = json.getString("url");
	gotHandshake_ = true;
}

inline void ArduinoCommunication::deleteDataForApi() {
	dataForApi = "";
	hasDataForApi_ = false;
}

inline void ArduinoCommunication::sendAck(){
	Serial.println(F("{\"type\":\"ACK\"}"));
}

inline void ArduinoCommunication::sendNack() {
	Serial.println(F("{\"type\":\"NACK\"}"));
}

inline void ArduinoCommunication::sendHandshake() {
	Serial.println(F("{\"handshake\":\"ESP\"}"));
}

inline void ArduinoCommunication::sendDataFromApi(const String & data) {
	Serial.println(data);
}

inline void ArduinoCommunication::run() {
	static String buff = "";

	if (Serial.available()) {
		char c = (char)Serial.read();
		if (c == END_LINE_CHAR) {
			processLine(buff);
			buff = "";
		}
		else {
			buff += c;
		}
	}
}