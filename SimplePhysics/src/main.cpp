#include <iostream>
#include <Windows.h>
#include "physics/PhysicsSystem.h"
#include "render/RenderSystem.h"
#include "component/transform.h"
#include "window/Window.h"
#include "render/Swapchain.h"
#include "entt/entt.hpp"

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