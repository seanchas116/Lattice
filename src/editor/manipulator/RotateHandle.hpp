#pragma once
#include "../../viewport/Renderable.hpp"

namespace Lattice {

namespace Mesh {
class Mesh;
}

namespace GL {
class VAO;
}

namespace Editor {
class MeshPicker;
namespace Manipulator {

class RotateHandle : public Viewport::Renderable, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    RotateHandle(int axis);

    void draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;
    void drawHitArea(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;

    void mousePressEvent(const Viewport::MouseEvent &event) override;
    void mouseMoveEvent(const Viewport::MouseEvent &event) override;
    void mouseReleaseEvent(const Viewport::MouseEvent &event) override;

    void contextMenuEvent(const Viewport::ContextMenuEvent &event) override;

    void setTargetPosition(const glm::dvec3 &targetPosition) { _targetPosition = targetPosition; }

signals:
    void onDragBegin(double value);
    void onDragMove(double value);
    void onDragEnd();

    void onContextMenu(const Viewport::ContextMenuEvent& event);

private:
    SP<Mesh::Mesh> createMesh();

    int _axis;
    glm::dvec3 _targetPosition {0};
    SP<Mesh::Mesh> _handleMesh;
    SP<GL::VAO> _handleVAO;
    glm::dvec3 _initialTargetPosition {0};
    bool _dragged = false;
};

}
} // namespace Editor
} // namespace Lattice
