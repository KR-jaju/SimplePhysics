#pragma once

#include <optional>
#include "types.h"
#include "entt/entt.hpp"
#include "math/vec.h"

class PhysicsSystem
{
public:
    PhysicsSystem(entt::registry& registry);
    void update(entt::registry& registry, float dt);

private:
    struct context
    {
        std::vector<collision_pair> candidates;
        std::vector<collision_pair> keys;
        std::vector<contact_info>   contacts;
    };

    void step(entt::registry& registry, float dt);
    void broadPhase(entt::registry& registry);
    void narrowPhase(context& context);
    void solve(entt::registry& registry);
    void integrate(entt::registry& registry, float dt);

    bool         gjk(const collision_pair& pair, std::vector<math::vec3>& simplex);
    contact_info epa(const collision_pair& pair, std::vector<math::vec3>& polytope);
};