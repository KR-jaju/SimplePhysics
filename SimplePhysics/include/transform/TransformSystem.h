#pragma once

#include "entt/entt.hpp"

class TransformSystem
{
public:
	TransformSystem(entt::registry& registry);
	void	update(entt::registry& registry, float aspectRatio);
};