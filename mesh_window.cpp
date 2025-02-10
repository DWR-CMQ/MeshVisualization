#include "mesh_window.h"
#include <string>
#include <iostream>

#include <QMutex>
#include <QFileDialog>
#include <QDropEvent>
#include <QMimeData>
#include <QSettings>
#include <QMessageBox>
#include <QColorDialog>
#include <QCoreApplication>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>

#include "core/surface_mesh.h"
#include "core/graph.h"
#include "core/poly_mesh.h"
#include "core/random.h"
#include "core/surface_mesh_builder.h"
#include "renderer/camera.h"
#include "renderer/renderer.h"
#include "renderer/clipping_plane.h"
#include "renderer/drawable_lines.h"
#include "renderer/drawable_points.h"
#include "renderer/key_frame_interpolator.h"
#include "renderer/drawable_triangles.h"
#include "renderer/manipulator.h"
#include "renderer/transform.h"
//#include "fileio/graph_io.h"
#include "fileio/surface_mesh_io.h"
#include "fileio/poly_mesh_io.h"
#include "fileio/translator.h"
#include "fileio/ply_reader_writer.h"

#include "util/logging.h"
#include "util/file_system.h"
#include "util/stop_watch.h"
#include "util/line_stream.h"
#include "util/version.h"

#include "paint_canvas.h"
#include "walk_through.h"
#include "ui/dialog/dialog_bilaterial_normal_filtering.h"
#include "ui/widget/widget_light_setting.h"

using namespace MV;

MeshWindow::MeshWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MeshProcessClass)
{
    ui->setupUi(this);
    ui->dockWidgetRendering->setFixedWidth(270);

    m_pViewer = new PaintCanvas(this);
    setCentralWidget(m_pViewer);

    auto widgetGlobalSetting = new WidgetLightSetting(this);
    ui->verticalLayout_light_setting->addWidget(widgetGlobalSetting);
    
    // setBaseSize(1024, 800);
    this->showMaximized();
    CreateActions();
    CreateMenus();
}

void MeshWindow::notify(std::size_t percent, bool update_viewer)
{
    //progress_bar_->setValue(int(percent));
    //cancelTaskButton_->setVisible(percent > 0 && percent < 100);
    //progress_bar_->setVisible(percent > 0 && percent < 100);

    if (update_viewer)
    {
        m_pViewer->update();
    }
        
    QApplication::processEvents();
}


void MeshWindow::send(el::Level level, const std::string& msg)
{
//    static QMutex mutex;
//    mutex.lock();
//    switch (level)
//    {
//    case el::Level::Info:
//        ui->listWidgetLog->addItem(QString::fromStdString("[INFO] " + msg));
//        // set to black will not work if it is dark mode
////            ui->listWidgetLog->item(ui->listWidgetLog->count() - 1)->setForeground(Qt::black);
//        break;
//    case el::Level::Warning:
//        ui->listWidgetLog->addItem(QString::fromStdString("[WARNING] " + msg));
//        ui->listWidgetLog->item(ui->listWidgetLog->count() - 1)->setForeground(Qt::darkRed);
//        break;
//    case el::Level::Error:
//        ui->listWidgetLog->addItem(QString::fromStdString("[ERROR] " + msg));
//        ui->listWidgetLog->item(ui->listWidgetLog->count() - 1)->setForeground(Qt::red);
//        break;
//    case el::Level::Fatal:  // no need to handle (app will crash)
//    default: break;
//    }
//
//    ui->listWidgetLog->scrollToBottom();
//    mutex.unlock();
}

Model* MeshWindow::open(const std::string& file_name)
{
    //SurfaceMesh mesh;

    //// add 4 vertices
    //const auto v0 = mesh.add_vertex(vec3(0, 0, 0));
    //const auto v1 = mesh.add_vertex(vec3(1, 0, 0));
    //const auto v2 = mesh.add_vertex(vec3(0, 1, 0));
    //const auto v3 = mesh.add_vertex(vec3(0, 0, 1));

    //// add 4 triangular faces
    //mesh.add_triangle(v0, v1, v3);
    //mesh.add_triangle(v1, v2, v3);
    //mesh.add_triangle(v2, v0, v3);
    //mesh.add_triangle(v0, v2, v1);

    //std::cout << "vertices: " << mesh.n_vertices() << std::endl;
    //std::cout << "edges: " << mesh.n_edges() << std::endl;
    //std::cout << "faces: " << mesh.n_faces() << std::endl;

    //float mean_valence = 0.0f;

    //// loop over all vertices
    //for (auto v : mesh.vertices())
    //{
    //    // sum up vertex valences
    //    mean_valence += mesh.valence(v);
    //}

    //mean_valence /= mesh.n_vertices();

    //std::cout << "mean valence: " << mean_valence << std::endl;



    auto models = m_pViewer->models();
    for (auto m : models)
    {
        if (m->name() == file_name) 
        {
            LOG(WARNING) << "model already loaded: " << file_name;
            return nullptr;
        }
    }

    const std::string& ext = file_system::extension(file_name, true);
    bool is_ply_mesh = false;
    if (ext == "ply")
    {
        is_ply_mesh = (io::PlyReader::num_instances(file_name, "face") > 0);
    }

    Model* model = nullptr;
    if ((ext == "ply" && is_ply_mesh) || ext == "obj" || ext == "off" || ext == "stl" || ext == "sm" || ext == "geojson" || ext == "trilist") 
    { 
        model = SurfaceMeshIO::load(file_name);
    }
    else if (ext == "ply" && io::PlyReader::num_instances(file_name, "edge") > 0)
    {
        //model = GraphIO::load(file_name);
    }
    else if (ext == "plm" || ext == "pm" || ext == "mesh")
    {
        model = PolyMeshIO::load(file_name);
    }
    else 
    { 
        // point cloud
    }

    if (model)
    {
        model->set_name(file_name);
        m_pViewer->addModel(model);
        //ui->treeWidgetModels->addModel(model, true);
        //setCurrentFile(QString::fromStdString(file_name));

        const auto keyframe_file = file_system::replace_extension(model->name(), "kf");
        if (file_system::is_file(keyframe_file)) 
        {
            if (m_pViewer->walkThrough()->interpolator()->read_keyframes(keyframe_file))
            {
                LOG(INFO) << "model has an accompanying animation file \'"
                    << file_system::simple_name(keyframe_file) << "\' (loaded)";
                m_pViewer->walkThrough()->set_scene({ model });
            }
        }
    }
    return model;
}


MeshWindow::~MeshWindow()
{}

void MeshWindow::CreateMenus()
{
    m_pMenuFile = menuBar()->addMenu(tr("File"));
    m_pMenuFile->addAction(m_pActionImportMesh);
    m_pMenuFile->addSeparator();

    m_pMenuAlgo = menuBar()->addMenu(tr("Algo"));
    m_pMenuAlgo->addAction(m_pActionBilateralNormalFiltering);
}

void MeshWindow::CreateActions()
{
    m_pActionImportMesh = new QAction(QIcon(":/Icons/open.ico"), tr("Import Mesh"), this);
    m_pActionImportMesh->setStatusTip("Import Mesh.");
    connect(m_pActionImportMesh, SIGNAL(triggered()), this, SLOT(ImportMesh()));

    m_pActionBilateralNormalFiltering = new QAction(QIcon(":/Icons/open.ico"), tr("BilateralNormalFiltering"), this);
    m_pActionBilateralNormalFiltering->setStatusTip("BilateralNormalFiltering.");
    connect(m_pActionBilateralNormalFiltering, SIGNAL(triggered()), this, SLOT(BilateralNormalFiltering()));
}

void MeshWindow::ImportMesh()
{
    std::string sFileName = QFileDialog::getOpenFileName(this, tr("Import Mesh"), "../Models", tr(" *.obj *.off *.ply *.stl")).toStdString();
    if (sFileName.empty())
    {
        return;
    }
    open(sFileName);
}

void MeshWindow::ExportMesh()
{
}

void MeshWindow::BilateralNormalFiltering()
{
    static DialogBilaterialNormalFiltering* dialog = nullptr;
    if (!dialog)
    {
        dialog = new DialogBilaterialNormalFiltering(this);
    }
    dialog->show();
}