#pragma once

#include "entt.hpp"
#include "Swapchain.h"

class RenderSystem
{
public:
	RenderSystem(entt::registry& registry, Swapchain* swapchain);
	void	update(entt::registry& registry);
private:
	struct context
	{
		Swapchain* swapchain;
	};
};