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
    void setTranslateHandleVisible(bool isTranslateHandleVisible);

    bool isRotateHandleVisible() const { return _isRotateHandleVisible; }
    void setRotateHandleVisible(bool isRotateHandleVisible);

    bool isScaleHandleVisible() const { return _isScaleHandleVisible; }
    void setScaleHandleVisible(bool isScaleHandleVisible);

signals:
    void targetPositionChanged(glm::dvec3 position);

    void translateHandleVisibleChanged(bool isTranslateHandleVisible);
    void rotateHandleVisibleChanged(bool isRotateHandleVisible);
    void scaleHandleVisibleChanged(bool isScaleHandleVisible);

    void onBegin(ValueType type, double value);
    void onChange(ValueType type, int axis, double value);
    void onEnd(ValueType type);

    void onContextMenu(const Render::ContextMenuEvent& event);

private:
    void updateChildren();

    std::vector<SP<Render::Renderable>> _translateHandles;
    std::vector<SP<Render::Renderable>> _rotateHandles;
    std::vector<SP<Render::Renderable>> _scaleHandles;
    glm::dvec3 _targetPosition;
    bool _isTranslateHandleVisible = true;
    bool _isRotateHandleVisible = false;
    bool _isScaleHandleVisible = false;
};

}
} // namespace Editor
} // namespace Lattice
