#include "Manipulator.hpp"
#include "ArrowHandle.hpp"
#include "CenterHandle.hpp"
#include "Manipulator.hpp"
#include "RotateHandle.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

Manipulator::Manipulator() : _centerHandle(makeShared<CenterHandle>()) {
    connect(this, &Manipulator::targetPositionChanged, _centerHandle.get(), &CenterHandle::setTargetPosition);
    connect(_centerHandle.get(), &CenterHandle::onDragBegin, this, [this](glm::dvec3 values) {
        emit onDragBegin(ValueType::Translate, values);
    });
    connect(_centerHandle.get(), &CenterHandle::onDragMove, this, [this](glm::dvec3 values) {
        emit onDragMove(ValueType::Translate, values);
    });
    connect(_centerHandle.get(), &CenterHandle::onDragEnd, this, [this]() {
        emit onDragEnd(ValueType::Translate);
    });
    connect(_centerHandle.get(), &CenterHandle::onContextMenu, this, &Manipulator::onContextMenu);

    for (int axis = 0; axis < 3; ++axis) {
        auto handle = makeShared<ArrowHandle>(axis, ArrowHandle::HandleType::Translate);
        handle->setTargetPosition(targetPosition());
        connect(this, &Manipulator::targetPositionChanged, handle.get(), &ArrowHandle::setTargetPosition);

        connect(handle.get(), &ArrowHandle::onDragBegin, this, [this, axis](double value) {
            glm::dvec3 values(0);
            values[axis] = value;
            emit onDragBegin(ValueType::Translate, values);
        });
        connect(handle.get(), &ArrowHandle::onDragMove, this, [this, axis](double value) {
            glm::dvec3 values(0);
            values[axis] = value;
            emit onDragMove(ValueType::Translate, values);
        });
        connect(handle.get(), &ArrowHandle::onDragEnd, this, [this] {
            emit onDragEnd(ValueType::Translate);
        });
        connect(handle.get(), &ArrowHandle::onContextMenu, this, &Manipulator::onContextMenu);
        _translateHandles.push_back(std::move(handle));
    }

    for (int axis = 0; axis < 3; ++axis) {
        auto handle = makeShared<ArrowHandle>(axis, ArrowHandle::HandleType::Scale);
        handle->setTargetPosition(targetPosition());
        connect(this, &Manipulator::targetPositionChanged, handle.get(), &ArrowHandle::setTargetPosition);

        connect(handle.get(), &ArrowHandle::onDragBegin, this, [this, axis](double value) {
            glm::dvec3 values(1);
            values[axis] = value;
            emit onDragBegin(ValueType::Scale, values);
        });
        connect(handle.get(), &ArrowHandle::onDragMove, this, [this, axis](double value) {
            glm::dvec3 values(1);
            values[axis] = value;
            emit onDragMove(ValueType::Scale, values);
        });
        connect(handle.get(), &ArrowHandle::onDragEnd, this, [this] {
            emit onDragEnd(ValueType::Scale);
        });
        connect(handle.get(), &ArrowHandle::onContextMenu, this, &Manipulator::onContextMenu);
        _scaleHandles.push_back(std::move(handle));
    }

    for (int axis = 0; axis < 3; ++axis) {
        auto handle = makeShared<RotateHandle>(axis);
        handle->setTargetPosition(targetPosition());
        connect(this, &Manipulator::targetPositionChanged, handle.get(), &RotateHandle::setTargetPosition);

        connect(handle.get(), &RotateHandle::onDragBegin, this, [this, axis](double value) {
            glm::dvec3 values(0);
            values[axis] = value;
            emit onDragBegin(ValueType::Rotate, values);
        });
        connect(handle.get(), &RotateHandle::onDragMove, this, [this, axis](double value) {
            glm::dvec3 values(0);
            values[axis] = value;
            emit onDragMove(ValueType::Rotate, values);
        });
        connect(handle.get(), &RotateHandle::onDragEnd, this, [this] {
            emit onDragEnd(ValueType::Rotate);
        });
        connect(handle.get(), &RotateHandle::onContextMenu, this, &Manipulator::onContextMenu);
        _rotateHandles.push_back(std::move(handle));
    }

    connect(this, &Manipulator::translateHandleVisibleChanged, this, &Manipulator::updateHandles);
    connect(this, &Manipulator::rotateHandleVisibleChanged, this, &Manipulator::updateHandles);
    connect(this, &Manipulator::scaleHandleVisibleChanged, this, &Manipulator::updateHandles);
    updateHandles();
}

void Manipulator::setTargetPosition(glm::dvec3 position) {
    if (_targetPosition == position) {
        return;
    }
    _targetPosition = position;
    emit targetPositionChanged(position);
}

void Manipulator::setTranslateHandleVisible(bool isTranslateHandleVisible) {
    if (_isTranslateHandleVisible == isTranslateHandleVisible) {
        return;
    }
    _isTranslateHandleVisible = isTranslateHandleVisible;
    emit translateHandleVisibleChanged(isTranslateHandleVisible);
}

void Manipulator::setRotateHandleVisible(bool isRotateHandleVisible) {
    if (_isRotateHandleVisible == isRotateHandleVisible) {
        return;
    }
    _isRotateHandleVisible = isRotateHandleVisible;
    emit rotateHandleVisibleChanged(isRotateHandleVisible);
}

void Manipulator::setScaleHandleVisible(bool isScaleHandleVisible) {
    if (_isScaleHandleVisible == isScaleHandleVisible) {
        return;
    }
    _isScaleHandleVisible = isScaleHandleVisible;
    emit scaleHandleVisibleChanged(isScaleHandleVisible);
}

void Manipulator::updateHandles() {
    std::vector<SP<Viewport::Renderable>> handles = {_centerHandle};
    if (_isTranslateHandleVisible) {
        handles.insert(handles.end(), _translateHandles.begin(), _translateHandles.end());
    }
    if (_isRotateHandleVisible) {
        handles.insert(handles.end(), _rotateHandles.begin(), _rotateHandles.end());
    }
    if (_isScaleHandleVisible) {
        handles.insert(handles.end(), _scaleHandles.begin(), _scaleHandles.end());
    }

    double translateHandleLength = _isRotateHandleVisible ? 2.3 : 2.0;
    double scaleHandleLength = (_isTranslateHandleVisible || _isRotateHandleVisible) ? 1.7 : 2.0;

    for (auto &translateHandle : _translateHandles) {
        translateHandle->setLength(translateHandleLength);
    }
    for (auto &scaleHandle : _scaleHandles) {
        scaleHandle->setLength(scaleHandleLength);
    }

    setChildRenderables(handles);
}

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
