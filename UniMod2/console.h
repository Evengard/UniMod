#pragma once

void console_init();
void print_to_console(const std::string& s, int color); // �������� � �������

extern int (__cdecl *nox_console_print)(int color,const wchar_t *text);
