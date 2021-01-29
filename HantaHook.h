#pragma once
#include <windows.h>
#include <d3d9.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <thread>
#include <assert.h>

#include "Util/xorstr.h"
#include "hooks/minhook/include/MinHook.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"

typedef long(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

#define UniText(text) L##text

namespace Hanta {
	inline std::uint32_t* g_methodsTable = NULL;
	void Destroy(); // destroy intself
};
