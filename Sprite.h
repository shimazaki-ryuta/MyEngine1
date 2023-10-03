#pragma once
#include "Vector2.h"
#include "Vector4.h"
#include "Matrix.h"

#include <d3d12.h>
#include <string>
#include <wrl.h>

class Sprite
{
public:

	enum class BlendMode
	{
		Normal
	};

	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
	};
	struct Material
	{
		Vector4 color;
		Matrix4x4 uvTransform;
	};

	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	// 頂点数
	static const int kVertNum = 4;
	// デバイス
	static ID3D12Device* sDevice;
	// デスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize;
	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList;
	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature;
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState;
	
	static void StaticInitialize(
		ID3D12Device* device, int window_width, int window_height,
		const std::wstring& directoryPath = L"Resources/");

	static void PreDraw(ID3D12GraphicsCommandList* cmdList);
	void Draw();
	static void PostDraw();

	static Sprite* Create(
		uint32_t textureHandle, Vector2 position, Vector2 size, Vector4 color = { 1, 1, 1, 1 });

	void Initialize();

	inline void SetPosition(const Vector2& pos) { position_ = pos; };

	inline void SetWVP(const Matrix4x4& wvp) { wvp_ = wvp; };

	inline void SetColor(const Vector4& color) { materialData_->color = color; };

	inline void SetUVTransform(const Matrix4x4& uvTransform) { uvTransform_ = uvTransform; };



private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	VertexData* vertexDataSprite_ = nullptr;
	uint32_t* indexData_ = nullptr;
	Material* materialData_ = nullptr;
	TransformationMatrix* transformationMatrixDataSprite = nullptr;
	Vector2 position_;
	Vector2 size_;
	Vector4 color_;
	uint32_t textureHandle_;

	Matrix4x4 uvTransform_;

	Matrix4x4 wvp_;

	D3D12_RESOURCE_DESC resourceDesc_;
};

