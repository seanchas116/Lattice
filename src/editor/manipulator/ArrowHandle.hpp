#pragma once
#include "../../render/Renderable.hpp"

namespace Lattice {

namespace OldGL {
class VAO;
class LineVAO;
}

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

    void setTargetPosition(const glm::dvec3 &targetPosition) { _targetPosition = targetPosition; }
    void setLength(double length) { _length = length; }

signals:
    void onBegin(double value);
    void onChange(double value);
    void onEnd();

private:
    SP<OldGL::VAO> createHandleVAO();
    SP<OldGL::LineVAO> createBodyVAO();

    int _axis;
    HandleType _handleType;
    glm::dvec3 _targetPosition {0};
    double _length {2.0};
    SP<OldGL::VAO> _handleVAO;
    SP<OldGL::LineVAO> _bodyVAO;
    glm::dvec3 _initialTargetPosition {0};
};

}
} // namespace Editor
} // namespace Lattice
