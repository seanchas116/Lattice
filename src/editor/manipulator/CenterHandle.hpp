#pragma once
#include "../../render/RenderableObject.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

class CenterHandle : public Render::RenderableObject {
    Q_OBJECT
public:
    CenterHandle();

    void draw(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;
    void drawPickables(const SP<Draw::Operations> &operations, const SP<Camera> &camera) override;
    void mousePressEvent(const Render::MouseEvent &event) override;
    void mouseMoveEvent(const Render::MouseEvent &event) override;
    void mouseReleaseEvent(const Render::MouseEvent &event) override;
    void contextMenuEvent(const Render::ContextMenuEvent &event) override;

    void setTargetPosition(glm::dvec3 pos) { _targetPosition = pos; }

signals:
    void onDragBegin(glm::dvec3 pos);
    void onDragMove(glm::dvec3 pos);
    void onDragEnd();

    void onContextMenu(const Render::ContextMenuEvent& event);

private:
    glm::dvec3 _targetPosition {0};
    double _depth = 0;
    bool _dragged = false;
};

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
