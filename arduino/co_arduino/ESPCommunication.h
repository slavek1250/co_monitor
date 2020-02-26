#pragma once
#include "GlobalDefinitions.h"
#include "DallasTherm.h"
#include "SafetyParams.h"
#include "Runnable.h"
#include "SimpleJSON.h"
#include "Loggable.h"

#define END_LINE_CHAR '\n'

class ESPCommunication :
	public Runnable
{	
	DallasTherm ** ds18b20;
	uint8_t ds18b20Number = 0;

	Loggable ** loggable;
	uint8_t loggableNum = 0;

	SafetyParams * safetyParams;

	String buff;

	uint32_t lastDataMs = 0;

	bool gotHandshake_ = false;
	bool gotConf_ = false;

	bool lastDataTransferGotAck = false;
	bool lastDataTransferGotNack = false;

	void processLine(String & line);
	void loadConfig(SimpleJSON & conf);

public:
	ESPCommunication() {};
	~ESPCommunication() {
		delete[]loggable;
		delete[]ds18b20;
	};

	void run() override;

	void addLoggable(Loggable * lg);
	void addDallasTermometerToConf(DallasTherm * ds);
	void setSafetyParametersToConf(SafetyParams * sp);


	void sendHandshake();
	bool gotHandshake();
	bool gotConfiguration();
	void sendData();

	uint32_t getLastDataMillis() { return lastDataMs; }
};

