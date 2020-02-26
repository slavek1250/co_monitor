#pragma once
#include "DisplayableValue.h"

#define NO_SPECIAL_CHAR 0xFF

class DisplayLine
{
	String title, unit;
	DisplayableValue * displayableValue = nullptr;

	uint8_t specialChar = NO_SPECIAL_CHAR;

public:
	DisplayLine(
		const String & title, 
		DisplayableValue * displayableValue = nullptr, 
		const String & unit = "", 
		uint8_t specialChar = NO_SPECIAL_CHAR
	) :
		title(title),
		displayableValue(displayableValue),
		unit(unit),
		specialChar(specialChar)

	{};
	~DisplayLine() {};

	bool hasSpecialChar() { 
		return specialChar != NO_SPECIAL_CHAR; 
	};
	uint8_t getSpecialChar() { 
		return specialChar; 
	}

	String getLineText() {
		if (displayableValue == nullptr)
			return title;
		else
			return (title + " " + displayableValue->getDisplayableValue() + unit);
	};
};