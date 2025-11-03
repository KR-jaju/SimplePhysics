#pragma once

#include "math/vec.h"

class Convex
{
  public:
    virtual ~Convex() = default;
    virtual float support(const vec3 &dir) const = 0;
};
