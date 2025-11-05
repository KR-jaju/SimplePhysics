#include "physics/PhysicsSystem.h"

#include <iostream>
#include "entt/entt.hpp"
#include "component/transform.h"

PhysicsSystem::PhysicsSystem(entt::registry& registry)
{
    registry.ctx().emplace<PhysicsSystem::context>();
}

void PhysicsSystem::update(entt::registry& registry, float dt)
{
    constexpr int substeps = 4;

    for (int i = 0; i < substeps; ++i)
    {
        this->step(registry, dt / substeps);
    }
}

void PhysicsSystem::step(entt::registry& registry, float dt)
{
    PhysicsSystem::context& ctx = registry.ctx().get<PhysicsSystem::context>();

    this->broadPhase(registry);
    this->narrowPhase(ctx);
    this->solve(registry);
    this->integrate(registry, dt);
}

void PhysicsSystem::broadPhase(entt::registry& registry)
{
    auto view = registry.view<transform>();
    for (auto [e, t] : view.each())
    {
        std::cout << sizeof(e) << ": " << t.position.x << std::endl;
    }
}

void PhysicsSystem::narrowPhase(context& context)
{
    std::vector<math::vec3> polytope;

    for (auto& pair : context.candidates)
    {
        polytope.clear();
        if (this->gjk(pair, polytope))
            continue;
        context.keys.push_back(pair);
        context.contacts.push_back(this->epa(pair, polytope));
    }
    // gjk
    // epa
    // generate islands
    // Implementation of the narrow phase collision detection
}

void PhysicsSystem::solve(entt::registry& registry)
{
    // Implementation of the physics solving step
}

void PhysicsSystem::integrate(entt::registry& registry, float dt)
{
    auto view = registry.view<transform>();
    for (auto [e, t] : view.each())
    {
        t.position += t.velocity * dt;
    }
}

bool PhysicsSystem::gjk(const collision_pair& pair, std::vector<math::vec3>& simplex)
{
    simplex.clear();
    // gjk algorithm implementation
    return false;
}

contact_info PhysicsSystem::epa(const collision_pair& pair, std::vector<math::vec3>& polytope)
{
    // epa algorithm implementation
    return {};
}