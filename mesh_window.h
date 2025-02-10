#pragma once

#include <QtWidgets/QMainWindow>
#include "util/progress.h"
#include "util/logging.h"
#include "core/types.h"
#include "core/model.h"
#include "ui_MeshProcess.h"

class PaintCanvas;

namespace Ui 
{
    class MeshProcessClass;
}

namespace MV 
{
    class Model;
    class Drawable;
}

class MeshWindow : public QMainWindow, public MV::ProgressClient, public MV::logging::Logger
{
    Q_OBJECT

public:
    MeshWindow(QWidget *parent = nullptr);
    ~MeshWindow();

public:
    MV::Model* open(const std::string& file_name);

private:
    void notify(std::size_t percent, bool update_viewer) override;
    void send(el::Level level, const std::string& msg) override;
    void CreateMenus();
    void CreateActions();

public:
    PaintCanvas* GetViewer() 
    { 
        return m_pViewer; 
    }

private:
    Ui::MeshProcessClass* ui;
    PaintCanvas* m_pViewer;

    // ÎÄ¼þ
    QMenu* m_pMenuFile;
    QAction* m_pActionImportMesh;
    QAction* m_pActionExportMesh;
    QAction* m_pActionExit;

    // Ëã·¨
    QMenu* m_pMenuAlgo;
    QAction* m_pActionBilateralNormalFiltering;

    //ioThread

private slots:
    void ImportMesh();
    void ExportMesh();
    void BilateralNormalFiltering();

private slots:
    //void SurfaceMeshBilateralNormalFiltering();
};
