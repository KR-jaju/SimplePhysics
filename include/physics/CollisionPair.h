#pragma once

#include "entt.hpp"

class CollisionPair
{
  public:
    CollisionPair(entt::entity a, entt::entity b) : entityA(a), entityB(b) {}

    entt::entity entityA;
    entt::entity entityB;
};