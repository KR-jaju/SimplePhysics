#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <cassert>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

// 간단한 DX11 디바이스 + 스왑체인 래퍼
// - D3D11CreateDeviceAndSwapChain() 경로만 사용 (DXGI 1.0 호환)
// - Flip Model 등은 쓰지 않고, DISCARD 스왑 이펙트로 최소화
class Swapchain {
public:
    struct Desc {
        UINT        width = 1280;
        UINT        height = 720;
        DXGI_FORMAT backbufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        DXGI_FORMAT depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        bool        vsync = true;
#ifdef _DEBUG
        bool        debug = true;
#else
        bool        debug = false;
#endif
    };

    void init(HWND hwnd, const Desc& d = {}) {
        hwnd_ = hwnd; desc_ = d;
        createDeviceAndSwapchain();
        createViews(desc_.width, desc_.height);
    }

    void resize(UINT w, UINT h) {
        if (!swap_) return;
        desc_.width = w ? w : 1;
        desc_.height = h ? h : 1;

        context_->OMSetRenderTargets(0, nullptr, nullptr);
        rtv_.Reset(); dsv_.Reset(); depth_.Reset();

        HRESULT hr = swap_->ResizeBuffers(0, desc_.width, desc_.height, DXGI_FORMAT_UNKNOWN, 0);
        assert(SUCCEEDED(hr));
        createViews(desc_.width, desc_.height);
    }

    void beginFrame(const float clear[4]) {
        context_->ClearRenderTargetView(rtv_.Get(), clear);
        if (dsv_) context_->ClearDepthStencilView(dsv_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        ID3D11RenderTargetView* rtvs[] = { rtv_.Get() };
        context_->OMSetRenderTargets(1, rtvs, dsv_.Get());

        D3D11_VIEWPORT vp{};
        vp.TopLeftX = 0.0f; vp.TopLeftY = 0.0f;
        vp.Width = static_cast<FLOAT>(desc_.width);
        vp.Height = static_cast<FLOAT>(desc_.height);
        vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
        context_->RSSetViewports(1, &vp);
    }

    void present() { swap_->Present(desc_.vsync ? 1 : 0, 0); }

    // Accessors
    ID3D11Device* device()  const { return device_.Get(); }
    ID3D11DeviceContext* context() const { return context_.Get(); }
    IDXGISwapChain* swapchain() const { return swap_.Get(); }
    ID3D11RenderTargetView* rtv() const { return rtv_.Get(); }
    ID3D11DepthStencilView* dsv() const { return dsv_.Get(); }

private:
    void createDeviceAndSwapchain() {
        DXGI_SWAP_CHAIN_DESC scd{};
        scd.BufferDesc.Width = desc_.width;
        scd.BufferDesc.Height = desc_.height;
        scd.BufferDesc.Format = desc_.backbufferFormat;
        scd.BufferDesc.RefreshRate = { 0, 0 }; // Present()의 스케줄에 따름
        scd.SampleDesc.Count = 1;  // MSAA 비활성(최소)
        scd.SampleDesc.Quality = 0;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.BufferCount = 1;       // DXGI 1.0 경로: 백버퍼 1개 (더블 버퍼링)
        scd.OutputWindow = hwnd_;
        scd.Windowed = TRUE;
        scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        scd.Flags = 0;

        UINT flags = 0;
        if (desc_.debug) flags |= D3D11_CREATE_DEVICE_DEBUG;

        D3D_FEATURE_LEVEL levels[] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0
        };
        D3D_FEATURE_LEVEL obtained{};

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
            levels, _countof(levels), D3D11_SDK_VERSION,
            &scd, &swap_, &device_, &obtained, &context_);
        assert(SUCCEEDED(hr));
    }

    void createViews(UINT w, UINT h) {
        // Backbuffer -> RTV
        ComPtr<ID3D11Texture2D> back;
        HRESULT hr = swap_->GetBuffer(0, IID_PPV_ARGS(&back));
        assert(SUCCEEDED(hr));
        hr = device_->CreateRenderTargetView(back.Get(), nullptr, &rtv_);
        assert(SUCCEEDED(hr));

        // Depth -> DSV
        D3D11_TEXTURE2D_DESC dd{};
        dd.Width = w; dd.Height = h; dd.MipLevels = 1; dd.ArraySize = 1;
        dd.Format = desc_.depthFormat;
        dd.SampleDesc.Count = 1; dd.SampleDesc.Quality = 0;
        dd.Usage = D3D11_USAGE_DEFAULT;
        dd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        hr = device_->CreateTexture2D(&dd, nullptr, &depth_); assert(SUCCEEDED(hr));
        hr = device_->CreateDepthStencilView(depth_.Get(), nullptr, &dsv_); assert(SUCCEEDED(hr));
    }

private:
    // Window
    HWND hwnd_ = nullptr;
    Desc desc_{};

    // Core objects
    ComPtr<ID3D11Device>        device_;
    ComPtr<ID3D11DeviceContext> context_;
    ComPtr<IDXGISwapChain>      swap_;

    // Views
    ComPtr<ID3D11RenderTargetView> rtv_;
    ComPtr<ID3D11Texture2D>        depth_;
    ComPtr<ID3D11DepthStencilView> dsv_;
};
