#pragma once

#include "../core/surface_mesh.h"
namespace MV
{
	class MeshSmooth
	{
	public:
		explicit MeshSmooth(SurfaceMesh* mesh);
		~MeshSmooth();

		void Initialize(bool bLaplace = false);
		void ExplicitSmooth(int iIters = 10, bool bLaplace = false);
		void ImplicitSmooth(float fTimeStep = 0.001, bool bLaplace = false, bool bRescale = true);

	private:
		void ComputeEdgeWeights(bool bLaplace);
		void ComputeVertexWeights(bool bLaplace);


	private:
		SurfaceMesh* m_pMesh;
		unsigned int m_uiMeshEdgeWeightsCount;
	};
}


