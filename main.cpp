#include <Windows.h>
#include "WindowProcedure.h"
#include <cstdint>
#include <string>
#include <format>
#include"ConvertString.h"

//directx12関係
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>


#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")


const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

void Log(const std::string& message)
{
	OutputDebugStringA(message.c_str());
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	
	WNDCLASS wc{};
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = L"CG2WindowClass";
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hCursor = LoadCursor(nullptr,IDC_ARROW);
	RegisterClass(&wc);

	RECT wrc = {0,0,kClientWidth,kClientHeight};
	AdjustWindowRect(&wrc,WS_OVERLAPPEDWINDOW,false);

	HWND hwnd = CreateWindow(wc.lpszClassName,
		L"CG2",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right-wrc.left,
		wrc.bottom-wrc.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr);
	//ウィンドウを表示
	ShowWindow(hwnd,SW_SHOW);

#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif 


	//DXGIファクトリーの生成
	IDXGIFactory7* dxgiFactory = nullptr;

	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));

	//アダプタ関係
	IDXGIAdapter4* useAdapter = nullptr;
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; i++)
	{
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));
		//ソフトウェアアダプタで無ければ
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			Log(ConvertString(std::format(L"USE Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;
	}
	assert(useAdapter != nullptr);

	//D3D12Deviceの生成
	ID3D12Device* device = nullptr;
	D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0};
	const char* featureLevelStrings[] = {"12.2","12.1","12.0"};
	for (size_t i=0;i<_countof(featureLevels);++i)
	{
		hr = D3D12CreateDevice(useAdapter,featureLevels[i],IID_PPV_ARGS(&device));
		if (SUCCEEDED(hr))
		{
			Log(std::format("FeatureLevel:{}\n", featureLevelStrings[i]));
			break;
		}
	}
	assert(device != nullptr);
	Log("Complete create D3D12Device!!!\n");

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION,true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR,true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING,true);
		infoQueue->Release();

		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = {D3D12_MESSAGE_SEVERITY_INFO};
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制
		infoQueue->PushStorageFilter(&filter);
	}
#endif // _DEBUG


	//コマンドキューの生成
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(&commandQueueDesc,IID_PPV_ARGS(&commandQueue));
	//コマンドキューの生成に失敗
	assert(SUCCEEDED(hr));

	//コマンドアロケータの生成
	ID3D12CommandAllocator* commandAllocator = nullptr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,IID_PPV_ARGS(&commandAllocator));
	//コマンドアロケータの生成に失敗
	assert(SUCCEEDED(hr));

	//コマンドリストの生成
	ID3D12GraphicsCommandList* commandList = nullptr;
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,commandAllocator, nullptr,IID_PPV_ARGS(&commandList));
	//コマンドリストの生成に失敗
	assert(SUCCEEDED(hr));


	//スワップチェーンの生成
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = kClientWidth;
	swapChainDesc.Height = kClientHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr,nullptr,reinterpret_cast<IDXGISwapChain1**>(&swapChain));
	assert(SUCCEEDED(hr));


	//ディスクリプタヒープの生成
	ID3D12DescriptorHeap* rtvDescriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvDexcriptoeHeapDesc{};
	rtvDexcriptoeHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDexcriptoeHeapDesc.NumDescriptors = 2;//ダブルバッファなので2
	hr = device->CreateDescriptorHeap(&rtvDexcriptoeHeapDesc,IID_PPV_ARGS(&rtvDescriptorHeap));
	//ディスクリプタヒープの生成に失敗
	assert(SUCCEEDED(hr));

	//SwapChainからResourceをひっぱってくる
	ID3D12Resource* swapChainResources[2] = {nullptr};
	hr = swapChain->GetBuffer(0,IID_PPV_ARGS(&swapChainResources[0]));
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1,IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));

	///RTVの作成
	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	//ディスクリプタの先頭取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//1つ目
	rtvHandles[0] = rtvStartHandle;
	device->CreateRenderTargetView(swapChainResources[0],&rtvDesc,rtvHandles[0]);
	//2つ目
	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	device->CreateRenderTargetView(swapChainResources[1],&rtvDesc,rtvHandles[1]);


	//初期値0でFenceを作る
	ID3D12Fence* fence = nullptr;
	uint64_t fenceValue = 0;
	hr = device->CreateFence(fenceValue,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));
	//Fence Signalを待つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	assert(fenceEvent != nullptr);

	MSG msg{};
	//メインループ
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//処理
			//画面の初期化
			//コマンドの確定
			UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
			
			//TransitionBarrierの設定
			D3D12_RESOURCE_BARRIER barrier{};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = swapChainResources[backBufferIndex];
			//遷移前
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			//遷移後
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			//TransitionBarrierを張る
			commandList->ResourceBarrier(1,&barrier);
			

			commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
			float clearColor[] = {0.1f,0.25f,0.5f,1.0f};//RGBA
			commandList->ClearRenderTargetView(rtvHandles[backBufferIndex],clearColor,0,nullptr);
			
			//描く処理終了、画面に移すため状態を遷移
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			//TransitionBarrierを張る
			commandList->ResourceBarrier(1, &barrier);
			
			//コマンドの確定(最後にやる)
			hr = commandList->Close();
			assert(SUCCEEDED(hr));

			//コマンドリストの実行
			ID3D12CommandList* commandLists[] = {commandList};
			commandQueue->ExecuteCommandLists(1,commandLists);
			swapChain->Present(1,0);

			//Fenceの値をこうしん
			fenceValue++;
			commandQueue->Signal(fence,fenceValue);

			//Fenceの値が指定したSignal値にたどり着いているか確認
			if (fence->GetCompletedValue()<fenceValue)
			{
				fence->SetEventOnCompletion(fenceValue,fenceEvent);
				//イベント待つ
				WaitForSingleObject(fenceEvent,INFINITE);
			}

			hr = commandAllocator->Reset();
			assert(SUCCEEDED(hr));
			hr = commandList->Reset(commandAllocator,nullptr);
			assert(SUCCEEDED(hr));

		}
	}

	Log(ConvertString(std::format(L"WSTRING:{}\n",msg.message)));

	CloseHandle(fenceEvent);
	fence->Release();
	rtvDescriptorHeap->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();
#ifdef _DEBUG
	debugController->Release();
#endif // _DEBUG
	CloseWindow(hwnd);


	//リソースリークチェック
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
	{
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}

	return 0;
}