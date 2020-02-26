#pragma once
#include <string.h>
class Loggable
{
public:
	Loggable() {};
	virtual ~Loggable() {};
	virtual String getLogName() = 0;
	virtual float getLogValue() = 0;
};

