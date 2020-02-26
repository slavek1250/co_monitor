#pragma once
#include "DisplayLine.h"

class DisplayPage
{
	DisplayLine _l0, _l1;

public:
	DisplayPage(const DisplayLine & l0 = DisplayLine(""), const DisplayLine & l1 = DisplayLine("")) : _l0(l0), _l1(l1) {};
	~DisplayPage() {};

	DisplayLine & getFistLine() {
		return _l0;
	}
	DisplayLine & getSecondLine() {
		return _l1;
	}
};