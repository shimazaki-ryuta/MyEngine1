#include "Sprite.h"
#include "ShaderCompiler.h"
#include "TextureManager.h"
#include "MatrixFunction.h"
ID3D12Device* Sprite::sDevice = nullptr;
UINT Sprite::sDescriptorHandleIncrementSize;
ID3D12GraphicsCommandList* Sprite::sCommandList = nullptr;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Sprite::sRootSignature;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Sprite::sPipelineState;

void Sprite::StaticInitialize(
	ID3D12Device* device, int window_width, int window_height, const std::wstring& directoryPath)
{
	sDevice = device;
	sDescriptorHandleIncrementSize =
		sDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	//dxCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//includeに対応するための設定を行う
	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

	//Shaderのコンパイル
	IDxcBlob* vertexShaderBlob = CompileShader(L"SpriteVS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);
	IDxcBlob* pixelShaderBlob = CompileShader(L"SpritePS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[3] = {};//0->ps用materia,1->vs用,2->texture用
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);


	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);



	//シリアライズしてバイナリ化
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr))
	{
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	//ID3D12RootSignature* rootSignature = nullptr;
	hr = sDevice->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&sRootSignature));
	assert(SUCCEEDED(hr));

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//02_04追加分
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendDtateの設定
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//PSOの生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = sRootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジのタイプ、三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//実際に生成
	//ID3D12PipelineState* graphicsPipelineState = nullptr;
	hr = sDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&sPipelineState));
	assert(SUCCEEDED(hr));
}

void Sprite::PreDraw(ID3D12GraphicsCommandList* commandList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Sprite::sCommandList == nullptr);

	// コマンドリストをセット
	sCommandList = commandList;

	// パイプラインステートの設定
	sCommandList->SetPipelineState(sPipelineState.Get());
	// ルートシグネチャの設定
	sCommandList->SetGraphicsRootSignature(sRootSignature.Get());
	sCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Sprite::PostDraw() {
	// コマンドリストを解除
	Sprite::sCommandList = nullptr;
}

Sprite* Sprite::Create(uint32_t textureHandle, Vector2 position, Vector2 size, Vector4 color)
{
	Sprite* sprite = new Sprite();

	sprite->textureHandle_ = textureHandle;
	sprite->position_ = position;
	sprite->size_ = size;
	sprite->color_ = color;

	sprite->Initialize();

	return sprite;
}

void Sprite::Initialize()
{
	resourceDesc_ = TextureManager::GetInstance()->GetResoureDesc(textureHandle_);

	//Sprite用のリソースを作る
	vertexResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(VertexData) * 4);
	//頂点バッファビューを作る
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite_));
	//1枚目
	vertexDataSprite_[0].position = { position_.x - size_.x/2.0f,position_.y - size_.y / 2.0f,0.0f,1.0f };
	vertexDataSprite_[0].texcoord = { 0.0f,0.0f };
	vertexDataSprite_[1].position = { position_.x+size_.x/2.0f,position_.y - size_.y / 2,0.0f,1.0f };
	vertexDataSprite_[1].texcoord = { 1.0f,0.0f };
	vertexDataSprite_[2].position = { position_.x - size_.x / 2.0f,position_.y + size_.y/2.0f,0.0f,1.0f };
	vertexDataSprite_[2].texcoord = { 0.0f,1.0f };
	vertexDataSprite_[3].position = { position_.x + size_.x/2.0f,position_.y + size_.y/2.0f,0.0f,1.0f };
	vertexDataSprite_[3].texcoord = { 1.0f,1.0f };

	//インデックス
	indexResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(uint32_t) * 6);

	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t)*6;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	indexData_[0] = 0;
	indexData_[1] = 1;
	indexData_[2] = 2;
	indexData_[3] = 1;
	indexData_[4] = 2;
	indexData_[5] = 3;

	transformResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(TransformationMatrix));
	
	transformResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	transformationMatrixDataSprite->WVP = MakeIdentity4x4();
	transformationMatrixDataSprite->World = MakeIdentity4x4();

	uvTransform_ = MakeIdentity4x4();

	//スプライト用のマテリアルリソースを作成
	materialResource_ = DirectXCommon::CreateBufferResource(sDevice, sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->uvTransform = uvTransform_;

}

void Sprite::Draw()
{
	Matrix4x4 worldMatrixSprite = MakeAffineMatrix(Vector3{ 0,0,0 }, Vector3{ 0,0,0 }, Vector3{ position_.x,position_.y,0.0f });
	Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(1280), float(720), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
	transformationMatrixDataSprite->WVP = wvp_;
	materialData_->uvTransform = uvTransform_;

	sCommandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2,textureHandle_);

	sCommandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	sCommandList->IASetIndexBuffer(&indexBufferView_);
	sCommandList->SetGraphicsRootConstantBufferView(1, transformResource_->GetGPUVirtualAddress());
	sCommandList->DrawIndexedInstanced(6,1,0,0,0);
}
