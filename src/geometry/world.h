#pragma once

#include "hop.h"
#include "geometry/shape.h"
#include "geometry/shape_instance.h"
#include "geometry/shape_manager.h"
#include "geometry/ray.h"
#include "geometry/hit_info.h"
#include "geometry/surface_interaction.h"
#include "material/material.h"
#include "accel/bvh_node.h"
#include "math/bbox.h"
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/transform.h"

#include "util/log.h"

#include <memory>
#include <vector>

namespace hop {

class World
{
public:
    ~World() { Log("world") << DEBUG << "world deleted"; }
    void add_shape(ShapeID shape_id);

    void preprocess();

    bool intersect(const Ray& r, HitInfo* hit) const;
    bool intersect_any(const Ray& r, HitInfo* hit) const;

    void get_surface_interaction(const HitInfo& hit, SurfaceInteraction* info);

    BBoxr get_bbox();

    // This will trigger a BBox recalculation when needed
    void set_dirty() { m_dirty = true; }

    bool empty() const { return m_instance_ids.empty(); }

private:
    void partition_instances();
    void partition_meshes();

private:
    std::vector<ShapeID> m_instance_ids;
    std::vector<bvh::Node> m_bvh_nodes;
    std::vector<Transformr> m_instance_inv_xfm;
    std::vector<uint32> m_instance_bvh_roots;
    std::vector<Vec3r> m_vertices;
    std::vector<Vec3r> m_normals;
    std::vector<Vec2r> m_uvs;
    std::vector<MaterialID> m_materials;
    bool m_dirty;
    BBoxr m_bbox;
};

} // namespace hop
