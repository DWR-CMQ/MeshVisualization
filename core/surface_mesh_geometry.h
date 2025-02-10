#ifndef EASY3D_ALGO_SURFACE_MESH_GEOMETRY_H
#define EASY3D_ALGO_SURFACE_MESH_GEOMETRY_H

#include "types.h"
#include "surface_mesh.h"

namespace MV 
{

    namespace geom {

        /** \brief compute area of triangle f 
        3*/
        float triangle_area(const SurfaceMesh *mesh, SurfaceMesh::Face f);

        /** \brief surface area of the mesh (assumes triangular faces)    */
        float surface_area(const SurfaceMesh *mesh);

        float face_area(const SurfaceMesh& mesh, SurfaceMesh::Face f);

        //! \brief Compute the volume of a mesh
        //! \details See \cite zhang_2002_efficient for details.
        //! \pre Input mesh needs to be a pure triangle mesh.
        float volume(const SurfaceMesh *mesh);

        /** \brief barycenter/centroid of a face    */
        vec3 centroid(const SurfaceMesh *mesh, SurfaceMesh::Face f);

        /** \brief barycenter/centroid of mesh, computed as area-weighted mean of vertices.    */
        /** \note assumes triangular faces.    */
        vec3 centroid(const SurfaceMesh *mesh);

        //! \brief Compute dual of a mesh.
        //! \warning Changes the mesh in place. All properties are cleared.
        void dual(SurfaceMesh *mesh);

        /** \brief compute the cotangent weight for edge e    */
        double cotan_weight(const SurfaceMesh *mesh, SurfaceMesh::Edge e);

        /** \brief compute (mixed) Voronoi area of vertex v    */
        double voronoi_area(const SurfaceMesh *mesh, SurfaceMesh::Vertex v);

        /** \brief compute barycentric Voronoi area of vertex v    */
        double voronoi_area_barycentric(const SurfaceMesh *mesh, SurfaceMesh::Vertex v);

        /** \brief compute Laplace vector for vertex v (normalized by Voronoi area)    */
        vec3 laplace(const SurfaceMesh *mesh, SurfaceMesh::Vertex v);

        /** \brief compute the sum of angles around vertex v (used for Gaussian curvature)    */
        double angle_sum(const SurfaceMesh *mesh, SurfaceMesh::Vertex v);

        /** \brief discrete curvature information for a vertex. used for vertex_curvature()    */
        struct VertexCurvature {
            VertexCurvature() : mean(0.0), gauss(0.0), max(0.0), min(0.0) {}

            float mean;
            float gauss;
            float max;
            float min;
        };

        /** \brief compute min, max, mean, and Gaussian curvature for vertex v. */
        /** \attention This will not give reliable values for boundary vertices.    */
        VertexCurvature vertex_curvature(const SurfaceMesh *mesh, SurfaceMesh::Vertex v);

    }

} // namespace easy3d


#endif  // EASY3D_ALGO_SURFACE_MESH_GEOMETRY_H