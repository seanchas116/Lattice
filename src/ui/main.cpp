#include <QApplication>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include "MainWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);

    QSurfaceFormat glFormat;
    glFormat.setVersion(3, 3);
    glFormat.setProfile(QSurfaceFormat::CoreProfile);
    glFormat.setDepthBufferSize(24);
    glFormat.setSamples(4);
    glFormat.setOption(QSurfaceFormat::DebugContext); // TODO: unset in release

    QSurfaceFormat::setDefaultFormat(glFormat);

    Lattice::UI::MainWindow window;
    window.show();
    return a.exec();
}