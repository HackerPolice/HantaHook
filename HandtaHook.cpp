#include "HantaHook.h"
#include "hook.h"

// Calling it when Dll is attaching
static void OnDllAttach(HMODULE hMod);
// main THread where all the hooking hapening
DWORD WINAPI MainThread();

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		OnDllAttach(hMod);
		break;
	case DLL_PROCESS_DETACH:
		Hanta::Destroy();
		break;
	}
	return TRUE;
}

static void OnDllAttach(HMODULE hMod) {
	DisableThreadLibraryCalls(hMod);
	std::thread mainThread(MainThread);
	mainThread.detach();
}

DWORD WINAPI MainThread()
{
	// HOoking our imageUi library
	return Hook::D3D9Hook();
}

void Hanta::Destroy() {

	MH_DisableHook(MH_ALL_HOOKS);
	free(g_methodsTable);
	g_methodsTable = NULL;
}