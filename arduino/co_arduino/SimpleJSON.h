#pragma once
#ifndef SIMPLE_JSON
#define SIMPLE_JSON

#define NULL_STR F("-1")

class SimpleJSON
{
	String & str;

public:
	SimpleJSON(String & JSON) : str(JSON) {};
	String getString(String name, int startFrom = 0);
	int32_t getInt(String name, int startFrom = 0);
	float getFloat(String name, int startFrom = 0);

	int getPossitionOf(const String & param, int startFrom = 0);
	~SimpleJSON() {};
};

inline String SimpleJSON::getString(String name, int startFrom)
{
	int16_t beginIte = str.indexOf(name, startFrom);
	if (beginIte == -1) {
		return NULL_STR;
	}

	beginIte = str.indexOf(":", beginIte);


	int16_t endIte = str.indexOf(",", beginIte);
	int16_t tmpIte = str.indexOf("}", beginIte);
	if (endIte == -1 && tmpIte != -1)	endIte = tmpIte;
	else if (endIte != -1 && tmpIte == -1) {}
	else if (endIte == -1 && tmpIte == -1)	endIte = str.length() - 1;
	else if (tmpIte < endIte)				endIte = tmpIte;


	if (beginIte == -1 || beginIte > endIte) {
		return NULL_STR;
	}
	++beginIte;
	if (str.charAt(endIte - 1) == '"') {
		++beginIte;
		--endIte;
	}

	return str.substring(beginIte, endIte);
}

inline int32_t SimpleJSON::getInt(String name, int startFrom)
{
	return getString(name, startFrom).toInt();
}

inline float SimpleJSON::getFloat(String name, int startFrom)
{
	return getString(name, startFrom).toFloat();
}

inline int SimpleJSON::getPossitionOf(const String & param, int startFrom)
{
	return str.indexOf(param, startFrom);;
}

#endif