#pragma once
#include <string>

namespace Console {
	enum Color {
		Grey = 2 };

	int print(const std::string& s, Console::Color color); // �������� � �������
	int print(const std::wstring& s, Console::Color color); // �������� � �������
	void init();

	extern int environment;
}