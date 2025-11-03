#pragma once
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <string>
#include <functional>
#include <cassert>

// Single-header Win32 Window wrapper
// - RAII registration/unregistration
// - Create/Show, basic message pump helpers
// - Callbacks: resize, close, paint, key, mouse (minimal)
// Usage:
//   Window win(L"My App", 1280, 720);
//   while (win.poll()) {
//       // render...
//   }

class Window {
public:
    struct Desc {
        std::wstring title = L"Window";
        UINT width = 1280;
        UINT height = 720;
        DWORD style = WS_OVERLAPPEDWINDOW;
        DWORD exStyle = 0;
        HICON hIcon = nullptr;
        HCURSOR hCursor = LoadCursor(nullptr, IDC_ARROW);
        HBRUSH hbrBackground = nullptr; // (HBRUSH)(COLOR_WINDOW+1)
    };

    // Callbacks (optional)
    std::function<void(UINT w, UINT h)> onResize;
    std::function<void()>                onClose;
    std::function<void()>                onPaint;
    std::function<void(UINT vk)>         onKeyDown;
    std::function<void(UINT vk)>         onKeyUp;

public:
    explicit Window(const Desc& d = {}) {
        registerClass(d);
        create(d);
        ShowWindow(m_hwnd, SW_SHOW);
        UpdateWindow(m_hwnd);
    }

    Window(const std::wstring& title, UINT w, UINT h) : Window(Desc{ title, w, h }) {}

    ~Window() {
        if (m_hwnd) {
            DestroyWindow(m_hwnd);
            m_hwnd = nullptr;
        }
        if (m_atom && m_hinst) {
            UnregisterClass(reinterpret_cast<LPCWSTR>(m_atom), m_hinst);
            m_atom = 0;
        }
    }

    // Non-copyable, movable
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&& rhs) noexcept { moveFrom(std::move(rhs)); }
    Window& operator=(Window&& rhs) noexcept { if (this != &rhs) { this->~Window(); moveFrom(std::move(rhs)); } return *this; }

    // Message pumps --------------------------------------------------
    // poll(): non-blocking. Returns false when WM_QUIT received.
    bool poll() {
        MSG msg{};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) return false;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return true;
    }

    // wait(): blocking - returns after one message processed; false on quit
    bool wait() {
        MSG msg{};
        if (GetMessage(&msg, nullptr, 0, 0) <= 0) return false; // WM_QUIT or error
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        return true;
    }

    // run(): simple loop that idles on no message (for renderers that want continuous redraw)
    int run(const std::function<void()>& frame) {
        MSG msg{};
        for (;;) {
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) return static_cast<int>(msg.wParam);
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            if (!IsWindowVisible(m_hwnd)) { Sleep(1); continue; }
            frame();
        }
    }

    HWND   hwnd()   const { return m_hwnd; }
    HINSTANCE hinst() const { return m_hinst; }
    UINT   width()  const { return m_clientW; }
    UINT   height() const { return m_clientH; }

private:
    // Per-instance window data
    static LRESULT CALLBACK s_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        Window* self = nullptr;
        if (msg == WM_NCCREATE) {
            auto cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            self = static_cast<Window*>(cs->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
            self->m_hwnd = hWnd;
        }
        else {
            self = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }
        if (self) return self->WndProc(msg, wParam, lParam);
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_SIZE: {
            m_clientW = LOWORD(lParam);
            m_clientH = HIWORD(lParam);
            if (onResize) onResize(m_clientW, m_clientH);
            return 0;
        }
        case WM_CLOSE:
            if (onClose) onClose();
            DestroyWindow(m_hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT:
            if (onPaint) onPaint();
            else {
                PAINTSTRUCT ps; BeginPaint(m_hwnd, &ps); EndPaint(m_hwnd, &ps);
            }
            return 0;
        case WM_KEYDOWN:
            if (onKeyDown) onKeyDown(static_cast<UINT>(wParam));
            return 0;
        case WM_KEYUP:
            if (onKeyUp) onKeyUp(static_cast<UINT>(wParam));
            return 0;
        }
        return DefWindowProc(m_hwnd, msg, wParam, lParam);
    }

    void registerClass(const Desc& d) {
        m_hinst = GetModuleHandle(nullptr);
        WNDCLASSEX wcex{};
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = &Window::s_WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = m_hinst;
        wcex.hIcon = d.hIcon;
        wcex.hCursor = d.hCursor;
        wcex.hbrBackground = d.hbrBackground;
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"WindowClass.RAII";
        wcex.hIconSm = d.hIcon;

        ATOM atom = RegisterClassEx(&wcex);
        if (!atom) {
            // Might be already registered by another instance; try to get atom via GetClassInfoEx
            WNDCLASSEX tmp{}; tmp.cbSize = sizeof(WNDCLASSEX);
            BOOL ok = GetClassInfoEx(m_hinst, wcex.lpszClassName, &tmp);
            assert(ok && "Failed to register or locate window class");
            atom = 1; // non-zero marker
        }
        m_atom = atom;
        m_className = wcex.lpszClassName;
    }

    void create(const Desc& d) {
        RECT rc{ 0,0,(LONG)d.width,(LONG)d.height };
        AdjustWindowRectEx(&rc, d.style, FALSE, d.exStyle);
        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;

        m_hwnd = CreateWindowEx(
            d.exStyle,
            m_className.c_str(),
            d.title.c_str(),
            d.style,
            CW_USEDEFAULT, CW_USEDEFAULT,
            w, h,
            nullptr, nullptr, m_hinst, this);
        assert(m_hwnd && "CreateWindowEx failed");

        // Initialize client size
        RECT cr{}; GetClientRect(m_hwnd, &cr);
        m_clientW = cr.right - cr.left;
        m_clientH = cr.bottom - cr.top;
    }

    void moveFrom(Window&& rhs) {
        m_hinst = rhs.m_hinst; rhs.m_hinst = nullptr;
        m_hwnd = rhs.m_hwnd;  rhs.m_hwnd = nullptr;
        m_atom = rhs.m_atom;  rhs.m_atom = 0;
        m_clientW = rhs.m_clientW; rhs.m_clientW = 0;
        m_clientH = rhs.m_clientH; rhs.m_clientH = 0;
        m_className = std::move(rhs.m_className);
    }

private:
    HINSTANCE   m_hinst = nullptr;
    HWND        m_hwnd = nullptr;
    ATOM        m_atom = 0;
    UINT        m_clientW = 0;
    UINT        m_clientH = 0;
    std::wstring m_className;
};
