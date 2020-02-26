#pragma once
#include "GlobalDefinitions.h"
#include <LiquidCrystal.h>
#include "Runnable.h"
#include "DisplayPage.h"

#define ROWS 2
#define COLS 16

enum DisplaySpecialChars {
	DEGREE = 223
};

template <int MAX_PAGES>
class Display :
	public LiquidCrystal,
	public Runnable
{
	DisplayPage pages[MAX_PAGES];
	uint8_t pagesNum = 0;
	uint8_t currentPage = 0;
	bool highlighted = true;

public:
	Display(const LiquidCrystal & lc) :
		LiquidCrystal(lc)
	{
		LiquidCrystal::begin(ROWS, COLS);
		pinMode(LCD_HIGHLIGHT_PIN, OUTPUT);
		digitalWrite(LCD_HIGHLIGHT_PIN, LCD_HIGHLIGHTED);
	}

	~Display() {};

	void run() override {
		static uint8_t l0LastLength = 0xFF, l1LastLength = 0xFF;


		String l0Text = pages[currentPage].getFistLine().getLineText();
		if (pages[currentPage].getFistLine().hasSpecialChar()) {
			l0Text.replace('?', (char)pages[currentPage].getFistLine().getSpecialChar());
		}

		String l1Text = pages[currentPage].getSecondLine().getLineText();
		if (pages[currentPage].getSecondLine().hasSpecialChar()) {
			l1Text.replace('?', (char)pages[currentPage].getSecondLine().getSpecialChar());
		}

		if (l0LastLength > l0Text.length() || l1LastLength > l1Text.length()) {
			LiquidCrystal::clear();
		}
		l0LastLength = l0Text.length();
		l1LastLength = l1Text.length();

		LiquidCrystal::setCursor(0, 0);
		LiquidCrystal::print(l0Text);
		LiquidCrystal::setCursor(0, 1);
		LiquidCrystal::print(l1Text);
	}

	void addPage(const DisplayPage & page) {
		if (pagesNum >= MAX_PAGES) {
			return;
		}
		pages[pagesNum++] = page;
	}


	void replacePage(uint8_t id, const DisplayPage & page) {
		if (id < this->pagesNum) {
			pages[id] = page;
		}
	}

	void showPage(uint8_t num) {
		currentPage = (num < pagesNum ? num : currentPage);
		run();
	}
	void showFirstPage() {
		showPage(0);
	}
	void showLastPage() {
		showPage(pagesNum - 1);
	}
	void showPreviousPage() {
		showPage(currentPage > 0 ? currentPage - 1 : pagesNum - 1);
	}
	void showNextPage(){
		showPage(currentPage != (pagesNum - 1) ? currentPage + 1 : 0);
	}
	
	bool isHighlighted() {
		return highlighted;
	}
	void displayHighlighted() {
		digitalWrite(LCD_HIGHLIGHT_PIN, LCD_HIGHLIGHTED);
		highlighted = true;
	}
	void displayDim() {
		digitalWrite(LCD_HIGHLIGHT_PIN, LCD_DIM);
		highlighted = false;
	}
	
};