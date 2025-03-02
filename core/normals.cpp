// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "normals.h"

namespace MV 
{

Normal face_normal(const SurfaceMesh& mesh, SurfaceMesh::Face f)
{
    SurfaceMesh::Halfedge h = mesh.halfedge(f);
    SurfaceMesh::Halfedge hend = h;

    auto vpoint = mesh.get_vertex_property<Point>("v:point");
    Point p0 = vpoint[mesh.target(h)];
    h = mesh.next(h);
    Point p1 = vpoint[mesh.target(h)];
    h = mesh.next(h);
    Point p2 = vpoint[mesh.target(h)];

    if (mesh.next(h) == hend) // face is a triangle
    {
        return normalize(cross(p2 -= p1, p0 -= p1));
    }
    else // face is a general polygon
    {
        Normal n(0, 0, 0);

        // this computes the sum of cross products (area-weighted normals)
        // of triangles generated by inserting the centroid c:
        //   sum_i (p_{i} - c) x (p_{i+1} - c)
        // The point c cancels out, leading to
        //   sum_i (p_{i} x p_{i+1}
        // This vector then has to be normalized.
        for (auto fh : mesh.halfedges(f))
        {
            n += cross(vpoint[mesh.source(fh)], vpoint[mesh.source(fh)]);
        }

        return normalize(n);
    }
}

Normal vertex_normal(const SurfaceMesh& mesh, SurfaceMesh::Vertex v)
{
    Point nn(0, 0, 0);

    if (!mesh.is_isolated(v))
    {
        auto vpoint = mesh.get_vertex_property<Point>("v:point");
        const Point p0 = vpoint[v];

        Normal n;
        Point p1, p2;
        float cosine, angle, denom;
        bool is_triangle;

        for (auto h : mesh.halfedges(v))
        {
            if (!mesh.is_border(h))
            {
                p1 = vpoint[mesh.target(h)];
                p1 -= p0;
                p2 = vpoint[mesh.source(mesh.prev(h))];
                p2 -= p0;

                // check whether we can robustly compute angle
                denom = sqrt(dot(p1, p1) * dot(p2, p2));
                if (denom > std::numeric_limits<float>::min())
                {
                    cosine = dot(p1, p2) / denom;
                    if (cosine < -1.0)
                        cosine = -1.0;
                    else if (cosine > 1.0)
                        cosine = 1.0;
                    angle = acos(cosine);

                    // compute triangle or polygon normal
                    is_triangle = (mesh.next(mesh.next(mesh.next(h))) == h);
                    n = is_triangle ? normalize(cross(p1, p2))
                                    : face_normal(mesh, mesh.face(h));

                    n *= angle;
                    nn += n;
                }
            }
        }

        nn = normalize(nn);
    }

    return nn;
}

Normal corner_normal(const SurfaceMesh& mesh, SurfaceMesh::Halfedge h, float crease_angle)
{
    // catch the two trivial cases
    if (crease_angle < 0.01)
        return face_normal(mesh, mesh.face(h));
    else if (crease_angle > 179)
        return vertex_normal(mesh, mesh.source(h));

    // avoid numerical problems
    if (crease_angle < 0.001)
        crease_angle = 0.001;

    const float cos_crease_angle = cos(crease_angle);
    Point nn(0, 0, 0);

    if (!mesh.is_border(h))
    {
        auto vpoint = mesh.get_vertex_property<Point>("v:point");

        const SurfaceMesh::Halfedge hend = h;
        const SurfaceMesh::Vertex v0 = mesh.target(h);
        const Point p0 = vpoint[v0];

        Point n, p1, p2;
        float cosine, angle, denom;
        bool is_triangle;

        // compute normal of h's face
        const Point nf = face_normal(mesh, mesh.face(h));

        // average over all incident faces
        do
        {
            if (!mesh.is_border(h))
            {
                p1 = vpoint[mesh.target(mesh.next(h))];
                p1 -= p0;
                p2 = vpoint[mesh.source(h)];
                p2 -= p0;

                // compute triangle or polygon normal
                is_triangle = (mesh.next(mesh.next(mesh.next(h))) == h);
                n = is_triangle ? normalize(cross(p1, p2))
                                : face_normal(mesh, mesh.face(h));

                // check whether normal is within crease_angle bound
                if (dot(n, nf) >= cos_crease_angle)
                {
                    // check whether we can robustly compute angle
                    denom = sqrt(dot(p1, p1) * dot(p2, p2));
                    if (denom > std::numeric_limits<float>::min())
                    {
                        cosine = dot(p1, p2) / denom;
                        if (cosine < -1.0)
                            cosine = -1.0;
                        else if (cosine > 1.0)
                            cosine = 1.0;
                        angle = acos(cosine);

                        n *= angle;
                        nn += n;
                    }
                }
            }
            h = mesh.opposite(mesh.next(h));
        } while (h != hend);

        nn = normalize(nn);
    }

    return nn;
}

void vertex_normals(SurfaceMesh& mesh)
{
    auto vnormal = mesh.vertex_property<Normal>("v:normal");
    for (auto v : mesh.vertices())
        vnormal[v] = vertex_normal(mesh, v);
}

void face_normals(SurfaceMesh& mesh)
{
    auto fnormal = mesh.face_property<Normal>("f:normal");
    for (auto f : mesh.faces())
        fnormal[f] = face_normal(mesh, f);
}


} // namespace pmp
