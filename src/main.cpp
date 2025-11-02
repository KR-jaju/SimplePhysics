
#include <iostream>
#include "physics/PhysicsSystem.h"
#include "component/transform.h"

using namespace std;

int main()
{
    entt::registry registry;
    PhysicsSystem  physicsSystem;

    entt::entity e = registry.create();

    // 컴포넌트 부착
    registry.emplace<::transform>(e);
    physicsSystem.update(registry, 0.016f);
    return 0;
}
