#pragma once
#include "../../viewport/Renderable.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

class CenterHandle : public Viewport::Renderable {
    Q_OBJECT
  public:
    CenterHandle();

    void draw(const Viewport::DrawEvent &event) override;
    void drawHitArea(const Viewport::DrawEvent &event) override;
    void mousePressEvent(const Viewport::MouseEvent &event) override;
    void mouseMoveEvent(const Viewport::MouseEvent &event) override;
    void mouseReleaseEvent(const Viewport::MouseEvent &event) override;
    void contextMenuEvent(const Viewport::MouseEvent &event) override;

    void setTargetPosition(glm::dvec3 pos) { _targetPosition = pos; }

  signals:
    void onDragBegin(glm::dvec3 pos);
    void onDragMove(glm::dvec3 pos);
    void onDragEnd();

    void onContextMenu(const Viewport::MouseEvent &event);

  private:
    glm::dvec3 _targetPosition{0};
    double _depth = 0;
    bool _dragged = false;
};

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
