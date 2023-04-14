#include <Windows.h>

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	OutputDebugStringA("Hello,Directx!\n");

	return 0;
}