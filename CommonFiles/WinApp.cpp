#include "WinApp.h"
#include"../ConvertString.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void Window::CreateGameWindow(const std::string& title, int32_t kClientWidth, int32_t kClientHeight)
{
	kClientWidth_ = kClientWidth;
	kClientHeight_ =  kClientHeight;

	wc_.lpfnWndProc = WindowProc;
	wc_.lpszClassName = L"CG2WindowClass";
	wc_.hInstance = GetModuleHandle(nullptr);
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);
	RegisterClass(&wc_);

	RECT wrc = { 0,0,kClientWidth,kClientHeight };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	hwnd_ = CreateWindow(wc_.lpszClassName,
		ConvertString(title).c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wc_.hInstance,
		nullptr);
	//ウィンドウを表示
	ShowWindow(hwnd_, SW_SHOW);
}

bool Window::ProcessMessage()
{
	MSG msg{};
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_QUIT)
	{
		return true;
	}
	return false;
}
