#include "geometry/triangle_mesh.h"
#include "geometry/triangle.h"
#include "geometry/hit_info.h"
#include "geometry/surface_interaction.h"
#include "math/bbox.h"
#include "math/math.h"

#include <string>
#include <utility>
#include <vector>

namespace hop {

TriangleMesh::TriangleMesh(const std::string& name,
                           std::vector<Triangle>& triangles)
    : m_name(name), m_triangles(std::move(triangles))
{
    m_bbox = BBoxr();
    for (auto& tri : m_triangles)
    {
        const BBoxr bbox = tri.get_bbox();
        m_bboxes.push_back(bbox);
        m_bbox.merge(bbox);
    }
    m_centroid = m_bbox.get_centroid();
}

void TriangleMesh::get_surface_interaction(const HitInfo& hit, SurfaceInteraction* info)
{
    Triangle& tri = m_triangles[hit.primitive_id];

    info->normal = (Real(1) - hit.b1 - hit.b2) * tri.normals[0] +
                   hit.b1 * tri.normals[1] + hit.b2 * tri.normals[2];
}

} // namespace hop
