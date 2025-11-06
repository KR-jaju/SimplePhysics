#include "pch.h"
#include "render/RenderSystem.h"
#include "render/Swapchain.h"
#include "component/transform.h"
#include "component/renderable.h"
#include "component/renderer.h"
#include "render/Shape.h"
#include <filesystem>
#include <iostream>



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
	const ComPtr<ID3D11Device> &device = swapchain->device();
	ComPtr<ID3DBlob> vsBlob = nullptr;
	ComPtr<ID3DBlob> psBlob = nullptr;
	ComPtr<ID3D11VertexShader> vs;
	ComPtr<ID3D11PixelShader> ps;
	ComPtr<ID3D11InputLayout> il;

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

	registry.ctx().emplace<RenderSystem::context>(context{
		.swapchain = swapchain,
		.shapeIL = std::move(il),
		.shapeVS = std::move(vs),
		.shapePS = std::move(ps)
		});
}

void	RenderSystem::update(entt::registry& registry)
{
	const float clear[4] = { 0.1f, 0.11f, 0.14f, 1.0f };
	context& ctx = registry.ctx().get<RenderSystem::context>();
	Swapchain* sc = ctx.swapchain;
	ComPtr<ID3D11DeviceContext> dc = ctx.swapchain->context();
	auto rendererView = registry.view<transform, renderer>();
	auto view = registry.view<transform, renderable>();

	ctx.swapchain->beginFrame(clear);
	dc->OMSetRenderTargets(0, sc->rtv().GetAddressOf(), sc->dsv().Get());
	for (auto [e, t, r] : rendererView.each())
	{
		r.projectionMatrix = r.computeProjectionMatrix();
		switch (r.rendererType)
		{
		case renderer::type::MainCamera:
			dc->VSSetShader(ctx.shapeVS.Get(), nullptr, 0);
			dc->PSSetShader(ctx.shapePS.Get(), nullptr, 0);
		case renderer::type::ShadowCaster:
			for (auto [e, t, r] : view.each())
			{
				r.shape->draw(dc); // 도형 그리기
			}
			break;
		}
		dc->VSSetShader(nullptr, nullptr, 0);
		dc->PSSetShader(nullptr, nullptr, 0);
	}
	ctx.swapchain->present();
}
