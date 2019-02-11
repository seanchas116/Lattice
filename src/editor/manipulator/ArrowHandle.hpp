#pragma once
#include "../../gl/VertexBuffer.hpp"
#include "../../render/Renderable.hpp"

namespace Lattice {

namespace Editor {
namespace Manipulator {

class ArrowHandle : public Render::Renderable {
    Q_OBJECT
public:
    enum class HandleType {
        Translate,
        Scale,
    };

    ArrowHandle(int axis, HandleType handleType);

    void draw(const SP<Render::Operations> &operations, const SP<Camera> &camera) override;

    std::optional<Render::HitResult> hitTest(glm::dvec2 pos, const SP<Camera> &camera) const override;
    void mousePress(const Render::MouseEvent &event) override;
    void mouseMove(const Render::MouseEvent &event) override;
    void mouseRelease(const Render::MouseEvent &event) override;
    void hoverEnter(const Render::MouseEvent& event) override;
    void hoverLeave() override;

    void setTargetPosition(const glm::dvec3 &targetPosition) { _targetPosition = targetPosition; }
    void setLength(double length) { _length = length; }

signals:
    void onBegin(double value);
    void onChange(double value);
    void onEnd();

private:
    SP<GL::VAO> createHandleVAO();
    SP<GL::VAO> createBodyVAO(const SP<GL::VertexBuffer<GL::Vertex>>& vertexBuffer);

    int _axis;
    HandleType _handleType;
    glm::dvec3 _targetPosition {0};
    double _length {2.0};
    SP<GL::VAO> _handleVAO;
    SP<GL::VertexBuffer<GL::Vertex>> _bodyVertexBuffer;
    SP<GL::VAO> _bodyVAO;
    glm::dvec3 _initialTargetPosition {0};
    bool _hovered = false;
};

}
} // namespace Editor
} // namespace Lattice
