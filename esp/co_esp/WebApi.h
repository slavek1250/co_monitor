#pragma once
#include <ESP8266HTTPClient.h>
#include "SimpleJSON.h"

class WebApi 
{
	String url, newConf;
	bool gotConfiguration_ = false;
	bool processApiResponse(String & data);

public:
	WebApi() {};
	~WebApi() {};

	void setUrl(const String & url) { this->url = url; }
	bool hasGotConfiguration() { return gotConfiguration_; }
	String getConfiguration() { return newConf; }
	
	bool sendData(const String & data);
	void fetchConfiguration();
	void deleteConfiguration();
};

inline bool WebApi::processApiResponse(String & data) {
	SimpleJSON json(data);

	String type = json.getString("type");
	if (type != NULL_STR) {
		if (type == "ACK") {
			return true;
		}
		else if (type == "CONF") {
			newConf = data;
			gotConfiguration_ = true;
			return true;
		}
	}
	return false;
}

bool WebApi::sendData(const String & data) {
	HTTPClient http;
	
	if (http.begin(url + String(API_SEND_DATA_FILE) + "?api_key=" + String(API_KEY))) {
		
		http.addHeader("Content-Type", "application/json");
		http.POST(data);

		String payLoad = http.getString();
		http.end();

		return processApiResponse(payLoad);
	}
	http.end();
	return false;
}

inline void WebApi::fetchConfiguration() {
	HTTPClient http;
	
	if (http.begin(url + String(API_CONFIG_FILE) + "?api_key=" + String(API_KEY))) {

		uint16_t httpCode = http.GET();
		if (httpCode == HTTP_CODE_OK) {
			newConf = http.getString();
			gotConfiguration_ = true;
		}
	}
	http.end();
}

inline void WebApi::deleteConfiguration() {
	newConf = "";
	gotConfiguration_ = false;
}
