#pragma once

#include <QWidget>
#include "../../core/types.h"
#include "ui_widget_light_setting.h"

namespace Ui
{
	class widget_light_setting;
}

class PaintCanvas;
class WidgetTrianglesDrawable;

class WidgetLightSetting : public QWidget
{
	Q_OBJECT

public:
    explicit WidgetLightSetting(QWidget *parent = nullptr);
	~WidgetLightSetting();

private:
	Ui::widget_light_setting* ui;
	PaintCanvas* m_pViewer;

	friend class MeshWindow;
};
