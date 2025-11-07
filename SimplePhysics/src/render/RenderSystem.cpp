#include "pch.h"
#include "render/RenderSystem.h"
#include "render/Swapchain.h"
#include "component/transform.h"
#include "component/renderable.h"
#include "component/renderer.h"
#include "render/Shape.h"
#include <filesystem>
#include <iostream>

struct alignas(16) PerRendererCB
{
	alignas(16) XMFLOAT4X4 vp;
	alignas(16) XMFLOAT3 worldCameraPosition;
};

struct PerRenderableCB
{

};

HRESULT CompileShaderFromFile(LPCTSTR szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	std::wcout << std::filesystem::current_path() << std::endl;
	ComPtr<ID3DBlob> pErrorBlob;
	hr = D3DCompileFromFile(
		szFileName,						// 파일명
		nullptr,						// 매크로 정의
		nullptr,						// Include 파일 정의
		szEntryPoint,					// 셰이더 Entry point 이름(메인 함수 이름)
		szShaderModel,					// 셰이더 컴파일 버전(vs_4_0 = Vertex shader version 4.0)
		dwShaderFlags,					// 컴파일 옵션
		0,								// 이펙트 컴파일 옵션
		ppBlobOut,						// 컴파일 된 Byte코드
		&pErrorBlob						// 에러 메세지
	);
	if (FAILED(hr))
	{
		if (pErrorBlob != nullptr)
			OutputDebugStringA(static_cast<LPCSTR>(pErrorBlob->GetBufferPointer()));
		return hr;
	}
	return S_OK;
}

RenderSystem::RenderSystem(entt::registry& registry, Swapchain* swapchain)
{
	static const D3D11_INPUT_ELEMENT_DESC shapeILDesc[] = {
		{
			.SemanticName = "POSITION",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = 0,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0
		},
	};
	assert(swapchain != nullptr);
	D3D11_BUFFER_DESC desc = {
		.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE
	};
	const ComPtr<ID3D11Device> &device = swapchain->device();
	ComPtr<ID3DBlob> vsBlob = nullptr;
	ComPtr<ID3DBlob> psBlob = nullptr;
	ComPtr<ID3D11VertexShader> vs;
	ComPtr<ID3D11PixelShader> ps;
	ComPtr<ID3D11InputLayout> il;
	ComPtr<ID3D11Buffer> perRendererCB;
	ComPtr<ID3D11Buffer> perRenderableCB;

	HRESULT hr = CompileShaderFromFile(L"shaders\\shapeVS.hlsl", "main", "vs_5_0", vsBlob.GetAddressOf());
	assert(SUCCEEDED(hr));
	hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vs);
	assert(SUCCEEDED(hr));
	hr = CompileShaderFromFile(L"shaders\\shapePS.hlsl", "main", "ps_5_0", &psBlob);
	assert(SUCCEEDED(hr));
	hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &ps);
	assert(SUCCEEDED(hr));
	hr = device->CreateInputLayout(
		shapeILDesc,
		sizeof(shapeILDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC),
		vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(),
		il.GetAddressOf()
	);
	assert(SUCCEEDED(hr));
	desc.ByteWidth = sizeof(PerRendererCB);
	hr = device->CreateBuffer(&desc, nullptr, &perRendererCB);
	desc.ByteWidth = 64;
	assert(SUCCEEDED(hr));
	hr = device->CreateBuffer(&desc, nullptr, &perRenderableCB);

	registry.ctx().emplace<RenderSystem::context>(context{
		.swapchain = swapchain,
		.shapeIL = std::move(il),
		.shapeVS = std::move(vs),
		.shapePS = std::move(ps),
		.perRendererCB = std::move(perRendererCB),
		.perRenderableCB = std::move(perRenderableCB)
		});
}

void	RenderSystem::update(entt::registry& registry)
{
	const float clear[4] = { 0.1f, 0.11f, 0.14f, 1.0f };
	ShapeRegistry& shapeRegistry = registry.ctx().get<ShapeRegistry>();
	context& ctx = registry.ctx().get<RenderSystem::context>();
	Swapchain* sc = ctx.swapchain;
	ComPtr<ID3D11DeviceContext> dc = ctx.swapchain->context();
	auto rendererView = registry.view<transform, renderer>();
	auto view = registry.view<transform, renderable>();

	ctx.swapchain->beginFrame(clear);
	//dc->OMSetRenderTargets(0, sc->rtv().GetAddressOf(), sc->dsv().Get());
	for (auto [e, t, r] : rendererView.each())
	{
		{
			D3D11_MAPPED_SUBRESOURCE mapped;

			dc->Map(ctx.perRendererCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
			PerRendererCB* data = static_cast<PerRendererCB*>(mapped.pData);
			data->vp = r.vp;
			data->worldCameraPosition = t.position;
			dc->Unmap(ctx.perRendererCB.Get(), 0);
		}
		switch (r.rendererType)
		{
		case renderer::type::MainCamera:
			dc->IASetInputLayout(ctx.shapeIL.Get());
			dc->VSSetShader(ctx.shapeVS.Get(), nullptr, 0);
			dc->PSSetShader(ctx.shapePS.Get(), nullptr, 0);
			dc->VSSetConstantBuffers(0, 1, ctx.perRendererCB.GetAddressOf());
			break;
		case renderer::type::ShadowCaster:
			dc->VSSetShader(ctx.shapeVS.Get(), nullptr, 0);
			break;
		}
		for (auto [e, t, r] : view.each())
		{
			//assert(r.shape != nullptr);
			Shape& shape = shapeRegistry.getShape(r.shape);
			{
				D3D11_MAPPED_SUBRESOURCE mapped;

				dc->Map(ctx.perRenderableCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
				memcpy(mapped.pData, &t.m, sizeof(XMFLOAT4X4));
				dc->Unmap(ctx.perRenderableCB.Get(), 0);
			}
			dc->VSSetConstantBuffers(1, 1, ctx.perRenderableCB.GetAddressOf());
			shape.draw(dc); // 도형 그리기
		}
		dc->VSSetShader(nullptr, nullptr, 0);
		//dc->VSSetConstantBuffers(0, 2, )
		dc->PSSetShader(nullptr, nullptr, 0);
	}
	ctx.swapchain->present();
}
