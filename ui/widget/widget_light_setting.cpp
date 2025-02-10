#include "widget_light_setting.h"
#include <QMouseEvent>
#include <QOpenGLFunctions>

#include "../../renderer/camera.h"
#include "../../renderer/drawable_triangles.h"
#include "../../renderer/ambient_occlusion.h"
#include "../../renderer/clipping_plane.h"
#include "../../renderer/manipulator.h"
#include "../../renderer/manipulated_frame.h"
#include "../../util/setting.h"

#include "../../../mesh_window.h"
#include "../../../paint_canvas.h"
#include <ui_widget_light_setting.h>

using namespace MV;

WidgetLightSetting::WidgetLightSetting(QWidget *parent) : QWidget(parent), ui(new Ui::widget_light_setting)
{
	ui->setupUi(this);
	m_pViewer = dynamic_cast<MeshWindow*>(parent)->GetViewer();

	QPalette::ColorRole role = parent->backgroundRole();
	const QColor& bc = parent->palette().color(role);
	ui->checkerSphere->SetBackgroundColor(bc);
	connect(ui->checkerSphere, SIGNAL(eLightPositionChanged()), m_pViewer, SLOT(update()));
}

WidgetLightSetting::~WidgetLightSetting()
{

}
