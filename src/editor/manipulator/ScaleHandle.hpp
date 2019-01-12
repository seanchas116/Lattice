#pragma once
#include <QObject>
#include "../../render/Renderable.hpp"

namespace Lattice {

namespace GL {
class VAO;
class LineVAO;
}

namespace Editor {
namespace Manipulator {

class ScaleHandle : public QObject, public Render::Renderable {
    Q_OBJECT
public:
    ScaleHandle(int axis);

    void draw(const SP<Render::Operations> &operations, const Camera &camera) override;

    std::optional<Render::HitResult> hitTest(glm::dvec2 pos, const Camera &camera) const override;
    void mousePress(QMouseEvent *event, glm::dvec2 pos, const Camera &camera, const Render::HitResult &hitResult) override;
    void mouseMove(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) override;
    void mouseRelease(QMouseEvent *event, glm::dvec2 pos, const Camera &camera) override;

    void setTargetPosition(const glm::dvec3 &targetPosition) { _targetPosition = targetPosition; }
    void setLength(double length) { _length = length; }

signals:
    void scaleStarted(double value);
    void scaleChanged(double value);
    void scaleFinished();

private:
    SP<GL::VAO> createHandleVAO();
    SP<GL::LineVAO> createBodyVAO();

    int _axis;
    glm::dvec3 _targetPosition {0};
    double _length {2.0};
    SP<GL::VAO> _handleVAO;
    SP<GL::LineVAO> _bodyVAO;
    glm::dvec3 _initialTargetPosition {0};
};

}
} // namespace Editor
} // namespace Lattice
