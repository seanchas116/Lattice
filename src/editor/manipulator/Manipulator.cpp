#include "Manipulator.hpp"
#include "ArrowHandle.hpp"
#include "RotateHandle.hpp"
#include "Manipulator.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

Manipulator::Manipulator() {
    for (int axis = 0; axis < 3; ++axis) {
        auto handle = makeShared<ArrowHandle>(axis, ArrowHandle::HandleType::Translate);
        handle->setTargetPosition(targetPosition());
        connect(this, &Manipulator::targetPositionChanged, handle.get(), &ArrowHandle::setTargetPosition);

        connect(handle.get(), &ArrowHandle::onBegin, this, [this] (double value) { emit onBegin(ValueType::Translate, value); });
        connect(handle.get(), &ArrowHandle::onChange, this, [this, axis] (double value) { emit onChange(ValueType::Translate, axis, value); });
        connect(handle.get(), &ArrowHandle::onEnd, this, [this] { emit onEnd(ValueType::Translate); });
        _translateHandles.push_back(std::move(handle));
    }

    for (int axis = 0; axis < 3; ++axis) {
        auto handle = makeShared<ArrowHandle>(axis, ArrowHandle::HandleType::Scale);
        handle->setTargetPosition(targetPosition());
        connect(this, &Manipulator::targetPositionChanged, handle.get(), &ArrowHandle::setTargetPosition);

        connect(handle.get(), &ArrowHandle::onBegin, this, [this] (double value) { emit onBegin(ValueType::Scale, value); });
        connect(handle.get(), &ArrowHandle::onChange, this, [this, axis] (double value) { emit onChange(ValueType::Scale, axis, value); });
        connect(handle.get(), &ArrowHandle::onEnd, this, [this] { emit onEnd(ValueType::Scale); });
        _scaleHandles.push_back(std::move(handle));
    }

    for (int axis = 0; axis < 3; ++axis) {
        auto handle = makeShared<RotateHandle>(axis);
        handle->setTargetPosition(targetPosition());
        connect(this, &Manipulator::targetPositionChanged, handle.get(), &RotateHandle::setTargetPosition);

        connect(handle.get(), &RotateHandle::onBegin, this, [this] (double value) { emit onBegin(ValueType::Rotate, value); });
        connect(handle.get(), &RotateHandle::onChange, this, [this, axis] (double value) { emit onChange(ValueType::Rotate, axis, value); });
        connect(handle.get(), &RotateHandle::onEnd, this, [this] { emit onEnd(ValueType::Rotate); });
        _rotateHandles.push_back(std::move(handle));
    }

    connect(this, &Manipulator::translateHandleVisibleChanged, this, &Manipulator::updateChildren);
    connect(this, &Manipulator::rotateHandleVisibleChanged, this, &Manipulator::updateChildren);
    connect(this, &Manipulator::scaleHandleVisibleChanged, this, &Manipulator::updateChildren);
    updateChildren();
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

void Manipulator::updateChildren() {
    std::vector<SP<Render::Renderable>> handles;
    if (_isTranslateHandleVisible) {
        for (auto& h : _translateHandles) {
            handles.push_back(h);
        }
    }
    if (_isRotateHandleVisible) {
        for (auto& h : _rotateHandles) {
            handles.push_back(h);
        }
    }
    if (_isScaleHandleVisible) {
        for (auto& h : _scaleHandles) {
            handles.push_back(h);
        }
    }
    setChildRenderables(handles);
}

}
} // namespace Editor
} // namespace Lattice
