#pragma once
#include <cassert>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <wrl.h>
struct D3DResourceLeakChacker
{
public:
	/*static D3DResourceLeakChacker* GetInstance()
	{
		static D3DResourceLeakChacker instance;
		return &instance;

	};*/
	//D3DResourceLeakChacker() = default;
	~D3DResourceLeakChacker()
	{
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
		{
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
			debug->Release();
		}
	};
	//private:
	//	D3DResourceLeakChacker() = default;
	//	//~D3DResourceLeakChacker() = default;
	//	D3DResourceLeakChacker(const D3DResourceLeakChacker&) = delete;
	//	D3DResourceLeakChacker operator=(const D3DResourceLeakChacker&) = delete;
};

struct LeackChecker {
	static D3DResourceLeakChacker checker;

};