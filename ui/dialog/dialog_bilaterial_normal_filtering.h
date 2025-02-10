#pragma once
#include "ui_dialog_bilaterial_normal_filtering.h"
#include <qdialog.h>

class MeshWindow;
class PaintCanvas;

class DialogBilaterialNormalFiltering : public QDialog, public Ui::BilaterialNormalFiltering
{
	Q_OBJECT
public:
	explicit DialogBilaterialNormalFiltering(MeshWindow* window);
	virtual ~DialogBilaterialNormalFiltering() {};

private Q_SLOTS:
	void apply();

protected:
	PaintCanvas* m_pViewer;
	MeshWindow* m_pWindow;
};

