#include <windows.h>
#pragma pack(1)



BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		MessageBox(
			NULL,
			L"HELLO WORLD!!!",
			L"UniMod2",
			MB_OK
			);
	}
	if (reason == DLL_PROCESS_DETACH)
	{

	}

	return 1;
}