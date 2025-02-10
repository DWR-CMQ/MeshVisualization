#include "dialog_bilaterial_normal_filtering.h"
#include "../mesh_window.h"
#include "../paint_canvas.h"
#include "../core/surface_mesh.h"
#include "../algo/bilaterial_denoise.h"
#include "../renderer/renderer.h"

using namespace MV;
DialogBilaterialNormalFiltering::DialogBilaterialNormalFiltering(MeshWindow* window)
{
	setupUi(this);
	m_pViewer = window->GetViewer();
	connect(okButton, SIGNAL(clicked()), this, SLOT(apply()));
}

void DialogBilaterialNormalFiltering::apply()
{
	auto mesh = dynamic_cast<SurfaceMesh*>(m_pViewer->currentModel());
	if (mesh == nullptr)
	{
		return;
	}
	BilaterialDenoise BilaterialDenoiseObj(DenoiseType::Bilaterial_Local);
	BilaterialDenoiseObj.Denoise(*mesh);
	mesh->renderer()->update();
	m_pViewer->update();
}