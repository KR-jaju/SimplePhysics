#pragma once

#include "vec3.h"

class Convex
{
  public:
    virtual ~Convex() = default;
    virtual float support(const vec3 &dir) const = 0;
}
