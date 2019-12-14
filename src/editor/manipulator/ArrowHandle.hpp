#pragma once
#include "../../viewport/Renderable.hpp"

namespace Lattice {

namespace Editor {
namespace Manipulator {

class ArrowHandle : public Viewport::Renderable {
    Q_OBJECT
  public:
    enum class HandleType {
        Translate,
        Scale,
    };

    ArrowHandle(int axis, HandleType handleType);

    void draw(const Viewport::DrawEvent &event) override;
    void drawHitArea(const Viewport::DrawEvent &event) override;

    void mousePressEvent(const Viewport::MouseEvent &event) override;
    void mouseMoveEvent(const Viewport::MouseEvent &event) override;
    void mouseReleaseEvent(const Viewport::MouseEvent &event) override;

    void contextMenuEvent(const Viewport::MouseEvent &event) override;

    void hoverEnterEvent(const Viewport::MouseEvent &event) override;
    void hoverLeaveEvent() override;

    void setTargetPosition(const glm::dvec3 &targetPosition) { _targetPosition = targetPosition; }
    void setLength(double length);

  signals:
    void onDragBegin(double value);
    void onDragMove(double value);
    void onDragEnd();

    void onContextMenu(const Viewport::MouseEvent &event);

  private:
    SP<GL::VAO> createHandleVAO();
    SP<GL::VAO> createBodyVAO(double length);
    SP<GL::VAO> createBodyPickVAO(double length);

    int _axis;
    HandleType _handleType;
    glm::dvec3 _targetPosition{0};
    double _length{2.0};
    SP<GL::VAO> _handleVAO;
    SP<GL::VAO> _bodyVAO;
    SP<GL::VAO> _bodyPickVAO;
    glm::dvec3 _initialTargetPosition{0};
    bool _hovered = false;
    bool _dragged = false;
};

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
