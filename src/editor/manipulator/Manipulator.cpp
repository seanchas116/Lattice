#include "Manipulator.hpp"
#include "ArrowHandle.hpp"
#include "RotateHandle.hpp"

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
}

void Manipulator::setTargetPosition(glm::dvec3 position) {
    if (_targetPosition == position) {
        return;
    }
    _targetPosition = position;
    emit targetPositionChanged(position);
}

}
} // namespace Editor
} // namespace Lattice
