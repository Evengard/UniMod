#pragma once
#include <string>

namespace Console {

	enum Color {
		Grey = 2 };

	void print(const std::string& s, Console::Color color); // печатает в консоль
	void init();
}