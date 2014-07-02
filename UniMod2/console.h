#pragma once

void console_init();
void print_to_console(const std::string& s, int color); // печатает в консоль

extern int (__cdecl *nox_console_print)(int color,const wchar_t *text);
