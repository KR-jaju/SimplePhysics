#include <iostream>
#include <Windows.h>
#include "PhysicsSystem.h"
#include "transform.h"

using namespace std;

int main()
{
    entt::registry registry;
    PhysicsSystem  physicsSystem(registry);

    entt::entity e = registry.create();

    // ÄÄÆ÷³ÍÆ® ºÎÂø
    registry.emplace<::transform>(e);
    physicsSystem.update(registry, 0.016f);

    getchar();
    return 0;
}