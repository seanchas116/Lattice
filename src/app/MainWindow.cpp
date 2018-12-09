#include "MainWindow.hpp"
#include "../viewport/ViewportWidget.hpp"

namespace Lattice {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    auto viewportWidget = new ViewportWidget();
    setCentralWidget(viewportWidget);
}

} // namespace Lattice
