#pragma once

#include "base_denoise.h"

namespace MV
{
void BilaterialDenoiseExecute(SurfaceMesh& mesh);

class BilaterialDenoise : public BaseDenoise
{
public:
    BilaterialDenoise(DenoiseType eType);
    ~BilaterialDenoise();
    void Denoise(SurfaceMesh& mesh) override;

public:
    void UpdateVertexPosition(SurfaceMesh& mesh, std::vector<Normal>& vecFilteredNormal, int iIterationNumber, bool bFixedBoundary = false);
    void LocalScheme(std::vector<Normal>& vecFilteredNormal);
    void GlobalScheme(std::vector<Normal>& vecFilteredNormal);

private:
    SurfaceMesh m_Mesh;
    std::vector<Normal> m_vecFaceNormal;
    std::vector<float> m_vecFaceArea;
    std::vector<Point> m_vecFaceCentroid;
    DenoiseType m_eDenoiseType;

private:
    double CaculateSigmaC(SurfaceMesh& mesh, std::vector<Point> vecFaceCentroid, double dValue);
    
};

}

