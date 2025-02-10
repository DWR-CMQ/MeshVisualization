#include "mesh_window.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <stdexcept>

#include <QDir>
#include <QTime>
#include <QApplication>
#include <QSplashScreen>
#include <QFileOpenEvent>

#include <QStyleFactory>
#include <QSurfaceFormat>
#include <QGLFormat>
#include <QElapsedTimer>
#include <QException>

#include "util/resource.h"
#include "util/initializer.h"
using namespace MV;

class Mapple : public QApplication 
{
public:
    Mapple(int& argc, char** argv) : QApplication(argc, argv) {}

    ~Mapple() override = default;

    bool notify(QObject* receiver, QEvent* event) override {
        try {
            return QApplication::notify(receiver, event);
        }
        catch (QException& e) {
            LOG(ERROR) << "caught an exception: " << e.what();
        }
        catch (std::exception& e) {
            LOG(ERROR) << "caught an exception: " << e.what();
        }
        catch (...) {
            LOG(ERROR) << "caught an unknown exception";
        }
        return false;
    }

    bool event(QEvent* e) override {
        switch (e->type()) {
        case QEvent::FileOpen:
            filesToOpen.push_back(dynamic_cast<QFileOpenEvent*>(e)->file());
            return true;
        default:
            return QApplication::event(e);
        }
    }

    QStringList filesToOpen;
};

int main(int argc, char *argv[])
{
    initialize(true, true, "E:/VS/QtInVS2019/MeshPro1/resources");

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setVersion(4, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setSamples(4);
#ifndef NDEBUG
    format.setOption(QSurfaceFormat::DebugContext);
#endif
    QSurfaceFormat::setDefaultFormat(format);

    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    Mapple app(argc, argv);

    if (!QGLFormat::hasOpenGL()) 
    {
        LOG(ERROR) << "Mapple needs OpenGL to run";
        return EXIT_FAILURE;
    }
    if ((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_3_2) == 0)
    {
        LOG(ERROR) << "Mapple needs OpenGL 3.2 at least to run";
        return EXIT_FAILURE;
    }

    MeshWindow win;
    win.show();

    QApplication::processEvents();
    std::string sFileName = "Noisy.obj";
    win.open(sFileName);
    QCoreApplication::processEvents();
    
    return app.exec();
}
