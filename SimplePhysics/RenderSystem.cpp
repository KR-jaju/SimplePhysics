#include "RenderSystem.h"
#include "Swapchain.h"

RenderSystem::RenderSystem(entt::registry& registry, Swapchain* swapchain)
{
	registry.ctx().emplace<RenderSystem::context>(context {.swapchain = swapchain});

}

void	RenderSystem::update(entt::registry& registry)
{
	const float clear[4] = { 0.1f, 0.11f, 0.14f, 1.0f };
	context& ctx = registry.ctx().get<RenderSystem::context>();

	ctx.swapchain->beginFrame(clear);
	ctx.swapchain->present();
}
