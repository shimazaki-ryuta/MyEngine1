#include "TextureManager.h"
#include "ConvertString.h"

TextureManager* TextureManager::GetInstance()
{
	static TextureManager instance;
	return &instance;
}


void TextureManager::Initialize(ID3D12Device* device, std::string directoryPath)
{
	device_ = device;
	directoryPath_ = directoryPath;

	descriptorHandleSize = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

DirectX::ScratchImage InputTexture(const std::string& filePath)
{
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	return mipImages;
}

ID3D12Resource* TextureManager::CreateTextureResource(const DirectX::TexMetadata& metadata)
{
	//metadataをもとにのResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	//利用するヒープの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device_->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&textures_[indexNextUseDiscriptorHeap].resource));
	assert(SUCCEEDED(hr));
	return resource;
}

uint32_t TextureManager::LoadTexture(const std::string& fileName)
{
	return TextureManager::GetInstance()->Load(fileName);
}

uint32_t TextureManager::Load(const std::string& fileName)
{
	//読み込み済Texture
	uint32_t handle = indexNextUseDiscriptorHeap;
	// 読み込み済みテクスチャを検索
	auto it = std::find_if(textures_.begin(), textures_.end(), [&](const auto& texture) {
		return texture.name == fileName;
		});
	if (it != textures_.end()) {
		// 読み込み済みテクスチャの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(textures_.begin(), it));
		return handle;
	}
	
	//新規Texture
	 // 書き込むテクスチャの参照
	Texture& texture = textures_.at(handle);
	texture.name = fileName;
	std::string filePath = directoryPath_ + fileName;

	DirectX::ScratchImage mipImages;// = new DirectX::ScratchImage();
	mipImages = InputTexture(filePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//ID3D12Resource* textureResource = CreateTextureResource(metadata);
	//texture.resource = CreateTextureResource(metadata);
	CreateTextureResource(metadata);
	dxCommon_->UploadTextureData(texture.resource.Get(), mipImages);
	//mipImages->Release();

	//metadataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	texture.srvHandleCPU = GetCPUDescriptorHandle(srvDescriptorHeap_, descriptorHandleSize, handle);
	texture.srvHandleGPU = GetGPUDescriptorHandle(srvDescriptorHeap_, descriptorHandleSize, handle);

	//textureSrvHandleCPU.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU.ptr += device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//SRVの生成
	device_->CreateShaderResourceView(texture.resource.Get(), &srvDesc, texture.srvHandleCPU);

	indexNextUseDiscriptorHeap++;

	return handle;
}

void TextureManager::SetGraphicsRootDescriptorTable(UINT rootParamIndex, uint32_t textureHandle)
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap_};
	dxCommon_->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	//SRVのDescriptorTableの先頭を設定。
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(rootParamIndex, textures_[textureHandle].srvHandleGPU);

}

const D3D12_RESOURCE_DESC TextureManager::GetResoureDesc(uint32_t textureHandle)
{
	assert(textureHandle < textures_.size());
	Texture& texture = textures_.at(textureHandle);
	return texture.resource->GetDesc();
}
