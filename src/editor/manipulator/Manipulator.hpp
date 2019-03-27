#pragma once
#include <QObject>
#include "../../render/RenderableObject.hpp"

namespace Lattice {

namespace Editor {
namespace Manipulator {

class Manipulator : public Render::RenderableObject {
    Q_OBJECT

public:
    enum class ValueType {
        Translate, Scale, Rotate
    };

    Manipulator();

    glm::dvec3 targetPosition() const { return _targetPosition; }
    void setTargetPosition(glm::dvec3 position);

    bool isTranslateHandleVisible() const { return _isTranslateHandleVisible; }
    void setIsTranslateHandleVisible(bool isTranslateHandleVisible);

    bool isRotateHandleVisible() const { return _isRotateHandleVisible; }
    void setIsRotateHandleVisible(bool isRotateHandleVisible);

    bool isScaleHandleVisible() const { return _isScaleHandleVisible; }
    void setIsScaleHandleVisible(bool isScaleHandleVisible);

    auto& translateHandles() const { return _translateHandles; }
    auto& rotateHandles() const { return _rotateHandles; }
    auto& scaleHandles() const { return _scaleHandles; }
    std::vector<SP<Render::RenderableObject>> handles(bool withTranslate, bool withRotate, bool withScale) const;

signals:
    void targetPositionChanged(glm::dvec3 position);

    void isTranslateHandleVisibleChanged(bool isTranslateHandleVisible);
    void isRotateHandleVisibleChanged(bool isRotateHandleVisible);
    void isScaleHandleVisibleChanged(bool isScaleHandleVisible);

    void onBegin(ValueType type, double value);
    void onChange(ValueType type, int axis, double value);
    void onEnd(ValueType type);

private:
    void updateChildren();

    std::vector<SP<Render::RenderableObject>> _translateHandles;
    std::vector<SP<Render::RenderableObject>> _rotateHandles;
    std::vector<SP<Render::RenderableObject>> _scaleHandles;
    glm::dvec3 _targetPosition;
    bool _isTranslateHandleVisible = true;
    bool _isRotateHandleVisible = true;
    bool _isScaleHandleVisible = true;
};

}
} // namespace Editor
} // namespace Lattice
