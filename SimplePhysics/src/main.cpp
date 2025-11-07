#include "pch.h"
#include <iostream>
#include <Windows.h>
#include "physics/PhysicsSystem.h"
#include "render/RenderSystem.h"
#include "transform/TransformSystem.h"

#include "component/transform.h"
#include "component/renderable.h"
#include "component/renderer.h"


#include "window/Window.h"
#include "render/Swapchain.h"
#include "entt/entt.hpp"
#include "core/ShapeRegistry.h"

using namespace std;

#include <vector>
#include <cstdint>
#include <cmath>
#include <DirectXMath.h>
using namespace DirectX;

int main()
{
    Window window(L"Hello Window", 1280, 720);
    Swapchain swapchain(window.hwnd(), { .width = window.width(), .height = window.height(), .vsync = true });
    entt::registry registry;
    ShapeRegistry   shapeRegistry = registry.ctx().emplace<ShapeRegistry>(swapchain.device());
    PhysicsSystem  physicsSystem(registry);
    RenderSystem   renderSystem(registry, &swapchain);
    TransformSystem transformSystem(registry);

    entt::entity e = registry.create();
    ::transform& t = registry.emplace<::transform>(e);
    {
        renderable& r = registry.emplace<::renderable>(e);
        r.shape = shapeRegistry.icosahedron();
        t.position.z = 1.5f;
    }
    {
        entt::entity e = registry.create();
        ::transform& t = registry.emplace<::transform>(e);
        renderer& r = registry.emplace<::renderer>(e);        
    }

    window.onKeyDown = [](UINT vk) { if (vk == VK_ESCAPE) PostQuitMessage(0); };
    float angle = 0;

    while (window.poll()) {
        angle += 0.01f;
        physicsSystem.update(registry, 0.016f);
        transformSystem.update(registry, swapchain.aspectRatio());
        renderSystem.update(registry);
        XMStoreFloat4(&t.rotation, XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), angle));
    }
    return 0;
}