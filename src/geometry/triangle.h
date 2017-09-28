#pragma once

#include "hop.h"
#include "types.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "material/material.h"

namespace hop {

class Triangle
{
public:
    Vec3r vertices[3];
    Vec3r normals[3];
    Vec2r uvs[3];
    MaterialID material_id;

    BBoxr bbox;
    Vec3r centroid;

    Triangle() : material_id(0) { }

    const BBoxr& get_bbox() const { return bbox; }
    const Vec3r& get_centroid() const { return centroid; }
};

} // namespace hop
