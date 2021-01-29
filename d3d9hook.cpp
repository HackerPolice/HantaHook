#include "hook.h"

HANDLE process_handle;

EndScene oEndScene = NULL;
WNDPROC oWndProc;
static HWND window = NULL;

void InitImGui(LPDIRECT3DDEVICE9 pDevice)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(pDevice);
}

bool init = false;
bool show = false;
static int fov = 90;

long __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	if (!init)
	{
		InitImGui(pDevice);
		init = true;
	}

	if (GetAsyncKeyState(VK_END)) { //  destroy 
		Hanta::Destroy();
		return 0;
	}

	if (GetAsyncKeyState(VK_INSERT) & 1 ) // return true when we will press Insert it will show our window and hide it
	{
		show = !show;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	
	ImGui::NewFrame();
	{
		ImVec2 size = ImGui::GetWindowSize();
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(size, ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.0f);
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowBorderSize = 0.0f;
		ImGui::Begin(XORS("IMGUI TEST"), (bool*)true, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize);
		{
			if (show) {
				ImGui::SliderInt("FOV Changer", &fov, -180, 180);
			}
		}
		ImGui::End();
	}
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	
	return oEndScene(pDevice);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	DWORD wndProcId;
	GetWindowThreadProcessId(handle, &wndProcId);

	if (GetCurrentProcessId() != wndProcId)
		return TRUE; // skip to next window

	window = handle;
	return FALSE; // window found abort search
}

HWND GetProcessWindow()
{
	window = NULL;
	EnumWindows(EnumWindowsCallback, NULL);
	return window;
}

static bool Init() { // checking for if we can create a window or not
	WNDCLASSEX windowClass;
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = UniText("HantaHook");
	windowClass.hIconSm = NULL;

	RegisterClassEx(&windowClass);

	HWND window = CreateWindow(windowClass.lpszClassName, UniText("DirectX Window"), WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, NULL, NULL, windowClass.hInstance, NULL);
	HMODULE libD3D9;
	if ((libD3D9 = ::GetModuleHandle(UniText("d3d9.dll"))) == NULL)
	{
		DestroyWindow(window);
		UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return false;
	}

	void* Direct3DCreate9;
	if ((Direct3DCreate9 = ::GetProcAddress(libD3D9, "Direct3DCreate9")) == NULL)
	{
		DestroyWindow(window);
		UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return false;
	}

	LPDIRECT3D9 direct3D9;
	if ((direct3D9 = ((LPDIRECT3D9(__stdcall*)(uint32_t))(Direct3DCreate9))(D3D_SDK_VERSION)) == NULL)
	{
		DestroyWindow(window);
		UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return false;
	}

	D3DDISPLAYMODE displayMode;
	if (direct3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode) < 0)
	{
		DestroyWindow(window);
		UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return false;
	}

	D3DPRESENT_PARAMETERS params;
	params.BackBufferWidth = 0;
	params.BackBufferHeight = 0;
	params.BackBufferFormat = displayMode.Format;
	params.BackBufferCount = 0;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.MultiSampleQuality = NULL;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.hDeviceWindow = window;
	params.Windowed = 1;
	params.EnableAutoDepthStencil = 0;
	params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	params.Flags = NULL;
	params.FullScreen_RefreshRateInHz = 0;
	params.PresentationInterval = 0;

	LPDIRECT3DDEVICE9 device;
	if (direct3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, &params, &device) < 0)
	{
		direct3D9->Release();
		::DestroyWindow(window);
		::UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
		return false;
	}

	Hanta::g_methodsTable = (std::uint32_t*)::calloc(119, sizeof(std::uint32_t));
	memcpy(Hanta::g_methodsTable, *(std::uint32_t**)device, 119 * sizeof(std::uint32_t));

	MH_Initialize();

	direct3D9->Release();
	direct3D9 = NULL;

	device->Release();
	device = NULL;

	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);

	return true;
}

static void bind(uint16_t _index, void** _original, void* _function) {
	assert(_index >= 0 && _original != NULL && _function != NULL);

	void* target = (void*)Hanta::g_methodsTable[_index];
	if (MH_CreateHook(target, _function, _original) != MH_OK || MH_EnableHook(target) != MH_OK)
	{
		return;
	}

	return;

}

DWORD Hook::D3D9Hook() {
	bool attached = false;
	do
	{
		if ( Init() )
		{
			bind(42, (void**)&oEndScene, hkEndScene);
			do
				window = GetProcessWindow();
			while (window == NULL);
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWL_WNDPROC, (LONG_PTR)WndProc);
			attached = true;
		}
	} while (!attached);
	return TRUE;
}