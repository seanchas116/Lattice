#include "Util.hpp"

namespace Lattice {
namespace Render {

double widgetPixelRatio(const QWidget *widget) {
#ifdef Q_OS_WIN
    return widget->logicalDpiX() / 96.0;
#else
    Q_UNUSED(widget)
    return 1.0;
#endif
}

glm::dvec2 mapQtToGL(const QWidget *widget, const QPoint &p) {
    return glm::dvec2(p.x(), widget->height() - p.y()) / widgetPixelRatio(widget);
}

} // namespace Render
} // namespace Lattice
