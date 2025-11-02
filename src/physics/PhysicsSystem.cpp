#include "physics/PhysicsSystem.h"

#include <iostream>
#include "entt.hpp"
#include "component/transform.h"

void PhysicsSystem::update(entt::registry &registry, float dt)
{
    constexpr int substeps = 4;

    for (int i = 0; i < substeps; ++i)
    {
        this->step(registry, dt / substeps);
    }
}

void PhysicsSystem::step(entt::registry &registry, float dt)
{
    this->broadPhase(registry);
    this->narrowPhase(registry);
    this->solve(registry);
    this->integrate(registry, dt);
}

void PhysicsSystem::broadPhase(entt::registry &registry)
{
    auto view = registry.view<transform>();
    for (auto [e, t] : view.each())
    {
        std::cout << sizeof(e) << ": " << t.position.x << std::endl;
    }
}

void PhysicsSystem::narrowPhase(entt::registry &registry)
{
    for (auto &pair : this->context.candidates)
    {
        std::cout << "Narrow phase for entities: " << int(pair.entityA) << " and "
                  << int(pair.entityB) << std::endl;
    }
    // gjk
    // epa
    // generate islands
    // Implementation of the narrow phase collision detection
}

void PhysicsSystem::solve(entt::registry &registry)
{
    // Implementation of the physics solving step
}

void PhysicsSystem::integrate(entt::registry &registry, float dt)
{
    auto view = registry.view<transform>();
    for (auto [e, t] : view.each())
    {
        t.position += t.velocity * dt;
    }
}