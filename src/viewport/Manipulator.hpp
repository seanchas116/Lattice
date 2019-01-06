#pragma once

#include <glm/glm.hpp>
#include <QObject>
#include "Operations.hpp"
#include "../support/Camera.hpp"

class QMouseEvent;

namespace Lattice::Viewport {

class Manipulator final : public QObject {
    Q_OBJECT
public:
    Manipulator();

    void draw(const SP<Operations>& operations, const Camera& camera);

    bool mousePress(QMouseEvent* event, glm::dvec2 pos, const Camera& camera);
    bool mouseMove(QMouseEvent* event, glm::dvec2 pos, const Camera& camera);
    bool mouseRelease(QMouseEvent* event, glm::dvec2 pos, const Camera& camera);

    glm::dvec3 targetPosition() const { return _targetPosition; }
    void setTargetPosition(glm::dvec3 pos) { _targetPosition = pos; }

    bool isTranslateHandleVisible() const { return _isTranslateHandleVisible; }
    void setIsTranslateHandleVisible(bool isTranslateHandleVisible) { _isTranslateHandleVisible = isTranslateHandleVisible; }

    bool isRotateHandleVisible() const { return _isRotateHandleVisible; }
    void setIsRotateHandleVisible(bool isRotateHandleVisible) { _isRotateHandleVisible = isRotateHandleVisible; }

    bool isScaleHandleVisible() const { return _isScaleHandleVisible; }
    void setIsScaleHandleVisible(bool isScaleHandleVisible) { _isScaleHandleVisible = isScaleHandleVisible; }

signals:
    void onDragStart();
    void onDrag(glm::dvec3 offset);
    void onDragEnd();

private:
    double translateHandleOffset() const;
    double scaleHandleOffset() const;
    double bodyEnd() const;

    SP<GL::VAO> _translateHandleVAO;
    SP<GL::VAO> _scaleHandleVAO;
    SP<GL::LineVAO> _rotateHandleVAO;
    SP<GL::LineVAO> _bodyVAO;
    SP<GL::PointVAO> _centerVAO;

    glm::dvec3 _targetPosition {0};

    bool _isTranslateHandleVisible = true;
    bool _isRotateHandleVisible = false;
    bool _isScaleHandleVisible = false;

    bool _isDragging = false;
    int _dragAxis = 0;
    glm::dvec3 _initialDragValue {0};
    glm::dvec3 _initialTargetPosition {0};
};

} // namespace Lattice
