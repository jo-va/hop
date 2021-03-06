#pragma once

#include "geometry/shape.h"
#include "geometry/shape_manager.h"
#include "math/bbox.h"
#include "math/transform.h"

#include <string>
#include <memory>

namespace hop {

class ShapeInstance : public Shape
{
public:
    ShapeInstance(ShapeID id, const Transformr& xfm, bool compute_tight_bbox);

    const std::string& get_name() const override { return m_name; }
    ShapeType get_type() const override { return m_shape->get_type(); };
    uint64 get_num_primitives() const override { return m_shape->get_num_primitives(); };
    bool is_instance() const override { return true; };
    const BBoxr& get_bbox() const override { return m_bbox; };
    const Vec3r& get_centroid() const override { return m_centroid; }

    Shape* get_shape() const { return m_shape; }

    const Transformr& get_transform() const { return m_transform; }

    bool transform_swaps_handedness() const { return m_transform_swaps_handedness; }

private:
    Shape* m_shape;
    ShapeID m_shape_id;
    Transformr m_transform;
    BBoxr m_bbox;
    Vec3r m_centroid;
    std::string m_name;
    bool m_transform_swaps_handedness;
};

typedef std::shared_ptr<ShapeInstance> ShapeInstancePtr;

} // namespace hop
