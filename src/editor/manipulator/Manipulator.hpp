#pragma once
#include <QObject>
#include "../../render/Renderable.hpp"

namespace Lattice {

namespace Editor {
namespace Manipulator {

class Manipulator : public QObject {
    Q_OBJECT
public:
    Manipulator();

    glm::dvec3 targetPosition() const { return _targetPosition; }
    void setTargetPosition(glm::dvec3 position);

    auto& translateHandles() const { return _translateHandles; }
    auto& scaleHandles() const { return _scaleHandles; }
    auto& rotateHandles() const { return _rotateHandles; }

    enum class ValueType {
        Translate, Scale, Rotate
    };

signals:
    void targetPositionChanged(glm::dvec3 position);
    void onBegin(ValueType type, double value);
    void onChange(ValueType type, int axis, double value);
    void onEnd(ValueType type);

private:
    std::vector<SP<Render::Renderable>> _translateHandles;
    std::vector<SP<Render::Renderable>> _scaleHandles;
    std::vector<SP<Render::Renderable>> _rotateHandles;
    glm::dvec3 _targetPosition;
};

}
} // namespace Editor
} // namespace Lattice
