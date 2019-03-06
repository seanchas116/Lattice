#include "Util.hpp"

namespace Lattice {
namespace Render {

glm::dvec2 mapQtToGL(const QWidget *widget, const QPoint &p) {
    return glm::dvec2(p.x(), widget->height() - p.y());
}

} // namespace Render
} // namespace Lattice
