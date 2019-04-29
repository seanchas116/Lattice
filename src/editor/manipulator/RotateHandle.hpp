#pragma once
#include "../../viewport/RenderableObject.hpp"

namespace Lattice {

namespace OldMesh {
class Mesh;
}

namespace GL {
class VAO;
}

namespace Editor {
class MeshPicker;
namespace Manipulator {

class RotateHandle : public Viewport::RenderableObject, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    RotateHandle(int axis);

    void draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;
    void drawPickables(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;

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
    SP<OldMesh::Mesh> createMesh();

    int _axis;
    glm::dvec3 _targetPosition {0};
    SP<OldMesh::Mesh> _handleMesh;
    SP<GL::VAO> _handleVAO;
    glm::dvec3 _initialTargetPosition {0};
    bool _dragged = false;
};

}
} // namespace Editor
} // namespace Lattice
