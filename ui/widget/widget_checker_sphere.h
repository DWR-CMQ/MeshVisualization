#pragma once

#include <QObject>
#include <QOpenGLWidget>
#include "../../core/types.h"

namespace MV
{
    class Camera;
    class TrianglesDrawable;
};

class WidgetCheckerSphere : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit WidgetCheckerSphere(QWidget* parent = nullptr);
    ~WidgetCheckerSphere();

    void SetBackgroundColor(const QColor& c) { m_BackgroundColor = c; }
    const QColor& GetBackgroundColor() const { return m_BackgroundColor; }

signals:
    void eLightPositionChanged();

public Q_SLOTS:
    void rResetLightPosition();

private slots:
    void mousePressEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

private:
    void CreateSpheres();
    void UpdateLighting();
    MV::vec3 ProjectToSphere(const QPoint& p);

    MV::Camera* m_pCamera;
    MV::TrianglesDrawable* m_pCheckerSphere;
    MV::TrianglesDrawable* m_pLightSphere;

    bool	 m_bMouseDown;
    float    m_fSceneRadius;
    QColor   m_BackgroundColor;

    MV::vec4	m_vecLightPos;
    MV::vec4	m_vecDefaultLightPos;
};

