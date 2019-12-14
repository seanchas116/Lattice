#pragma once
#include "../../viewport/Renderable.hpp"

namespace meshlib {
class Mesh;
}

namespace Lattice {

namespace GL {
class VAO;
}

namespace Editor {
class MeshPicker;
namespace Manipulator {

class RotateHandle : public Viewport::Renderable {
    Q_OBJECT
  public:
    RotateHandle(int axis);

    void draw(const Viewport::DrawEvent &event) override;
    void drawHitArea(const Viewport::DrawEvent &event) override;

    void mousePressEvent(const Viewport::MouseEvent &event) override;
    void mouseMoveEvent(const Viewport::MouseEvent &event) override;
    void mouseReleaseEvent(const Viewport::MouseEvent &event) override;

    void contextMenuEvent(const Viewport::MouseEvent &event) override;

    void setTargetPosition(const glm::dvec3 &targetPosition) { _targetPosition = targetPosition; }

  signals:
    void onDragBegin(double value);
    void onDragMove(double value);
    void onDragEnd();

    void onContextMenu(const Viewport::MouseEvent &event);

  private:
    SP<meshlib::Mesh> createMesh();

    int _axis;
    glm::dvec3 _targetPosition{0};
    SP<meshlib::Mesh> _handleMesh;
    SP<GL::VAO> _handleVAO;
    glm::dvec3 _initialTargetPosition{0};
    bool _dragged = false;
};

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
