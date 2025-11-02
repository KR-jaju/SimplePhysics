#pragma once

#include "math/vec.h"

struct transform
{
    math::vec3 position;
    math::vec3 rotation;
    math::vec3 scale;
    math::vec3 velocity;
};