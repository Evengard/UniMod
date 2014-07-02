#include <Windows.h>
#include "memory.h"

void inject_addr(DWORD offset, void* fn)
{
	const  int size = 4; // размер адреса 
	DWORD *addr = (DWORD*)(offset+1); // на следующую инструкцию от call
 	DWORD old_protect;
	VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &old_protect);
	*addr = (DWORD) fn;
	VirtualProtect(addr, size, old_protect, &old_protect);
}
void inject_offs(DWORD offset, void* fn)
{
	const  int size = 4; // размер адреса 
	DWORD *addr = (DWORD*)(offset+1); // на следующую инструкцию от call
	DWORD delta = DWORD(fn) - DWORD(addr) - 4;
 	DWORD old_protect;
	VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &old_protect);
	*addr = delta;
	VirtualProtect(addr, size, old_protect, &old_protect);
}
void inject_jump(DWORD offset, void* fn)
{
	const  int size = 5; // размер адреса 

	BYTE *jump = (BYTE*)offset;
	DWORD *addr = (DWORD*)(jump+1);

	DWORD delta = DWORD(fn) - DWORD(addr) - 4;

 	DWORD old_protect;
	VirtualProtect(jump, size, PAGE_EXECUTE_READWRITE, &old_protect);
	*jump = 0xE9;
	*addr = delta;
	VirtualProtect(jump, size, old_protect, &old_protect);
}

void write_array(DWORD offset, BYTE* buffer, int size)
{
	DWORD old_protect;
	DWORD *addr = (DWORD*)offset;
	VirtualProtect(addr, size, PAGE_EXECUTE_READWRITE, &old_protect);
	memcpy(addr,(void*)buffer, size);
	VirtualProtect(addr, size, old_protect, &old_protect);
}