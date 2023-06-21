#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
//ImGui
#include "../externals/imgui/imgui.h"
#include "../externals/imgui/imgui_impl_dx12.h"
#include "../externals/imgui/imgui_impl_win32.h"

class Window
{
public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	/// <summary>
	/// ゲームウィンドウの作成
	/// </summary>
	void CreateGameWindow(const std::string& title,int32_t kClientWidth,int32_t kClientHight);
	
	/// <summary>
	/// メッセージの処理
	/// </summary>
	/// <returns>終了かどうか</returns>
	bool ProcessMessage();

	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	inline HWND GetHwnd() { return hwnd_; };

private:
	WNDCLASS wc_{};
	HWND hwnd_;

};