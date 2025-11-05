#pragma once

#include "entt/entt.hpp"

class collision_pair
{
public:
    collision_pair() = default;
    collision_pair(entt::entity a, entt::entity b) : entityA(a), entityB(b) {}

    entt::entity entityA = entt::null;
    entt::entity entityB = entt::null;
};

struct contact_info
{
};
