#pragma once
#include "../../gl/VertexBuffer.hpp"
#include "../../render/RenderableObject.hpp"

namespace Lattice {

namespace Editor {
namespace Manipulator {

class ArrowHandle : public Render::RenderableObject {
    Q_OBJECT
public:
    enum class HandleType {
        Translate,
        Scale,
    };

    ArrowHandle(int axis, HandleType handleType);

    void draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;
    void drawPickables(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;

    void mousePress(const Render::MouseEvent &event) override;
    void mouseMove(const Render::MouseEvent &event) override;
    void mouseRelease(const Render::MouseEvent &event) override;
    void hoverEnter(const Render::MouseEvent& event) override;
    void hoverLeave() override;

    void setTargetPosition(const glm::dvec3 &targetPosition) { _targetPosition = targetPosition; }
    void setLength(double length);

signals:
    void onBegin(double value);
    void onChange(double value);
    void onEnd();

private:
    SP<GL::VAO> createHandleVAO();
    SP<GL::VAO> createBodyVAO(double length);
    SP<GL::VAO> createBodyPickVAO(double length);

    int _axis;
    HandleType _handleType;
    glm::dvec3 _targetPosition {0};
    double _length {2.0};
    SP<GL::VAO> _handleVAO;
    SP<GL::VAO> _bodyVAO;
    SP<GL::VAO> _bodyPickVAO;
    glm::dvec3 _initialTargetPosition {0};
    bool _hovered = false;
    bool _dragged = false;
};

}
} // namespace Editor
} // namespace Lattice
