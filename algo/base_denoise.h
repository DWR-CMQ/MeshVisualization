#pragma once
#include "../core/surface_mesh.h"
#include "../core/normals.h"
#include "../core/surface_mesh_geometry.h"

namespace MV 
{

enum class DenoiseType
{
    Bilaterial_Local,
    Bilaterial_Global,
    Unknown
};

class BaseDenoise
{
public:
    BaseDenoise(){};
    ~BaseDenoise(){};

public:
    virtual void Denoise(SurfaceMesh& mesh) = 0;

public:
    std::vector<Normal> GetFaceNormal(const SurfaceMesh& mesh)
    {
        std::vector<Normal> vecFaceNormal(mesh.faces_size());
        for (auto f : mesh.faces())
        {
            vecFaceNormal[f.idx()] = face_normal(mesh, f);
        }
        return vecFaceNormal;
    }

    std::vector<float> GetFaceArea(const SurfaceMesh& mesh)
    {
        std::vector<float> vecFaceArea(mesh.faces_size());
        for (auto f : mesh.faces())
        {
            vecFaceArea[f.idx()] = geom::face_area(mesh, f);
        }
        return vecFaceArea;
    }

    std::vector<vec3> GetFaceCentroid(const SurfaceMesh& mesh)
    {
        std::vector<vec3> vecFaceCentroid(mesh.faces_size());
        for (auto f : mesh.faces())
        {
            vecFaceCentroid[f.idx()] = geom::centroid(&mesh, f);
        }
        return vecFaceCentroid;
    }
};

}

