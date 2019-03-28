#pragma once
#include "../../render/RenderableObject.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

class CenterPickable : public Render::RenderableObject {
    Q_OBJECT
public:
    CenterPickable();

    void draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;
    void drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;
    void mousePressEvent(const Render::MouseEvent &event) override;
    void mouseMoveEvent(const Render::MouseEvent &event) override;
    void mouseReleaseEvent(const Render::MouseEvent &event) override;
    void contextMenuEvent(const Render::ContextMenuEvent &event) override;

    void setTargetPosition(glm::dvec3 pos) { _targetPosition = pos; }

signals:
    void onContextMenu(const Render::ContextMenuEvent& event);

private:
    glm::dvec3 _targetPosition {0};
};

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
