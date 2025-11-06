#include "pch.h"
#include <iostream>
#include <Windows.h>
#include "physics/PhysicsSystem.h"
#include "render/RenderSystem.h"
#include "component/transform.h"
#include "window/Window.h"
#include "render/Swapchain.h"
#include "entt/entt.hpp"
#include "core/ShapeRegistry.h"

using namespace std;

int main()
{
    Window window(L"Hello Window", 1280, 720);
    Swapchain swapchain(window.hwnd(), { .width = window.width(), .height = window.height(), .vsync = true });
    entt::registry registry;
    ShapeRegistry   shapeRegistry = registry.ctx().emplace<ShapeRegistry>(swapchain.device());
    PhysicsSystem  physicsSystem(registry);
    RenderSystem   renderSystem(registry, &swapchain);

    entt::entity e = registry.create();
    registry.emplace<::transform>(e);

    window.onKeyDown = [](UINT vk) { if (vk == VK_ESCAPE) PostQuitMessage(0); };

    while (window.poll()) {
        physicsSystem.update(registry, 0.016f);
        renderSystem.update(registry);
    }
    return 0;
}