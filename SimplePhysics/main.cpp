#include <iostream>
#include <Windows.h>
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "transform.h"
#include "Window.h"
#include "Swapchain.h"

using namespace std;

int main()
{
    Window window(L"Hello Window", 1280, 720);
    Swapchain swapchain;
    entt::registry registry;
    PhysicsSystem  physicsSystem(registry);
    RenderSystem   renderSystem(registry, &swapchain);

    entt::entity e = registry.create();

    swapchain.init(window.hwnd(), { .width = window.width(), .height = window.height(), .vsync = true });
    registry.emplace<::transform>(e);
    physicsSystem.update(registry, 0.016f);

    window.onKeyDown = [](UINT vk) { if (vk == VK_ESCAPE) PostQuitMessage(0); };

    while (window.poll()) {
        physicsSystem.update(registry, 0.016f);
        renderSystem.update(registry);
    }
    return 0;
}