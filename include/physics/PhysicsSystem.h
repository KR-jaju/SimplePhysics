#pragma once

#include <optional>
#include "CollisionPair.h"
#include "entt.hpp"

class PhysicsSystem
{
  public:
    void update(entt::registry &registry, float dt);

  private:
    struct Context
    {
        std::vector<CollisionPair> candidates;
        std::vector<CollisionPair> keys;
        // std::vector<
    };
    Context context;

    void step(entt::registry &registry, float dt);
    void broadPhase(entt::registry &registry);
    void narrowPhase(entt::registry &registry);
    void solve(entt::registry &registry);
    void integrate(entt::registry &registry, float dt);
};