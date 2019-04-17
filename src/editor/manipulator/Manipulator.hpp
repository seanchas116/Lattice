#pragma once
#include <QObject>
#include "../../viewport/RenderableObject.hpp"

namespace Lattice {

namespace Editor {
namespace Manipulator {

class CenterHandle;
class ArrowHandle;
class RotateHandle;

class Manipulator : public Viewport::RenderableObject {
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

    void onDragBegin(ValueType type, glm::dvec3 values);
    void onDragMove(ValueType type, glm::dvec3 values);
    void onDragEnd(ValueType type);

    void onContextMenu(const Viewport::ContextMenuEvent& event);

private:
    void updateHandles();

    SP<CenterHandle> _centerHandle;
    std::vector<SP<ArrowHandle>> _translateHandles;
    std::vector<SP<RotateHandle>> _rotateHandles;
    std::vector<SP<ArrowHandle>> _scaleHandles;
    glm::dvec3 _targetPosition;
    bool _isTranslateHandleVisible = true;
    bool _isRotateHandleVisible = false;
    bool _isScaleHandleVisible = false;
};

}
} // namespace Editor
} // namespace Lattice
