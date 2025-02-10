#include "mesh_smooth.h"
#include "../core/surface_mesh_geometry.h"
#include <iostream>
#include <math.h>

namespace MV
{
	MeshSmooth::MeshSmooth(SurfaceMesh* mesh)
	{
		m_pMesh = mesh;
		m_uiMeshEdgeWeightsCount = 0;
	}

	MeshSmooth::~MeshSmooth()
	{

	}

	void MeshSmooth::Initialize(bool bLaplace)
	{
		ComputeEdgeWeights(bLaplace);
		ComputeVertexWeights(bLaplace);
	}

	void MeshSmooth::ExplicitSmooth(int iIters, bool bLaplace)
	{
		if (!m_pMesh->n_vertices())
		{
			return;
		}

		auto eweight = m_pMesh->get_edge_property<float>("e:cotan");
		if (!eweight || m_uiMeshEdgeWeightsCount != m_pMesh->n_edges())
		{
			ComputeEdgeWeights(bLaplace);
		}
		eweight = m_pMesh->get_edge_property<float>("e:cotan");
		auto points = m_pMesh->get_vertex_property<vec3>("v:point");
		auto laplace = m_pMesh->get_vertex_property<vec3>("v:laplace");

		// 滤波迭代
		SurfaceMesh::Vertex vv;
		SurfaceMesh::Edge e;
		for (int i = 0; i < iIters; i++)
		{
			// step1 对每一个顶点计算laplace权重
			for (auto item : m_pMesh->vertices())
			{
				vec3 temp(0, 0, 0);
				if (!m_pMesh->is_border(item))
				{
					float w(0.0);
					for (auto h : m_pMesh->halfedges(item))
					{
						
					}
				}
			}
		}



	}

	void MeshSmooth::ImplicitSmooth(float fTimeStep, bool bLaplace, bool bRescale)
	{

	}

	void MeshSmooth::ComputeEdgeWeights(bool bLaplace)
	{
		auto eweight = m_pMesh->edge_property<float>("e:cotan");
		if (bLaplace)
		{
			for (auto item : m_pMesh->edges())
			{
				eweight[item] = 1.0;
			}
		}
		else
		{
			for (auto item : m_pMesh->edges())
			{
				eweight[item] = static_cast<float>(std::max(0.0, geom::cotan_weight(m_pMesh, item)));
			}
		}
		m_uiMeshEdgeWeightsCount = m_pMesh->n_edges();
	}

	void MeshSmooth::ComputeVertexWeights(bool bLaplace)
	{
		auto vweight = m_pMesh->vertex_property<float>("v:area");
		if (bLaplace)
		{
			for (auto item : m_pMesh->vertices())
			{
				vweight[item] = static_cast<float>(1.0 / m_pMesh->valence(item));
			}
		}
		else
		{
			for (auto item : m_pMesh->vertices())
			{
				//vweight[item] = static_cast<float>(0.5 / m_pMesh->valence(item));
			}
		}

	}
}

