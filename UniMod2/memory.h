#pragma once
#include <Windows.h>

void inject_addr(DWORD offset, void* fn);
void inject_jump(DWORD offset, void* fn);

void write_array(DWORD offset, BYTE* buffer, int size);

