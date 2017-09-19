#pragma once

#include "hop.h"
#include "math/math.h"
#include "math/vec2.h"
#include "math/vec3.h"

namespace hop {

class SurfaceInteraction
{
public:
    Vec3r position;
    Vec3r normal;
    Vec2r uv;
};

} // namespace hop
