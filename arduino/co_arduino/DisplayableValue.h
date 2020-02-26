#pragma once
class DisplayableValue
{
public:
	DisplayableValue() {};
	virtual ~DisplayableValue() {};
	virtual String getDisplayableValue() = 0;
};