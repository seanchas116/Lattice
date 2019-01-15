#include "ObjectManipulator.hpp"
#include "ArrowHandle.hpp"
#include "RotateHandle.hpp"
#include "../../ui/AppState.hpp"
#include "../../document/Document.hpp"
#include "../../document/Item.hpp"
#include "../../document/History.hpp"
#include "../../support/OptionalGuard.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

ObjectManipulator::ObjectManipulator(const SP<UI::AppState> &appState) : _appState(appState)
{
    connectToItem(appState->document()->currentItem());
    connect(appState->document().get(), &Document::Document::currentItemChanged, this, &ObjectManipulator::connectToItem);

    for (int axis = 0; axis < 3; ++axis) {
        auto handle = makeShared<ArrowHandle>(axis, ArrowHandle::HandleType::Translate);
        handle->setTargetPosition(position());
        connect(this, &ObjectManipulator::positionChanged, handle.get(), &ArrowHandle::setTargetPosition);

        connect(handle.get(), &ArrowHandle::onBegin, this, [this] (double value) { handleOnBegin(ValueType::Translate, value); });
        connect(handle.get(), &ArrowHandle::onChange, this, [this, axis] (double value) { handleOnChange(ValueType::Translate, axis, value); });
        connect(handle.get(), &ArrowHandle::onEnd, this, [this] { handleOnEnd(ValueType::Translate); });
        _translateHandles.push_back(std::move(handle));
    }

    for (int axis = 0; axis < 3; ++axis) {
        auto handle = makeShared<ArrowHandle>(axis, ArrowHandle::HandleType::Scale);
        handle->setTargetPosition(position());
        connect(this, &ObjectManipulator::positionChanged, handle.get(), &ArrowHandle::setTargetPosition);

        connect(handle.get(), &ArrowHandle::onBegin, this, [this] (double value) { handleOnBegin(ValueType::Scale, value); });
        connect(handle.get(), &ArrowHandle::onChange, this, [this, axis] (double value) { handleOnChange(ValueType::Scale, axis, value); });
        connect(handle.get(), &ArrowHandle::onEnd, this, [this] { handleOnEnd(ValueType::Scale); });
        _scaleHandles.push_back(std::move(handle));
    }

    for (int axis = 0; axis < 3; ++axis) {
        auto handle = makeShared<RotateHandle>(axis);
        handle->setTargetPosition(position());
        connect(this, &ObjectManipulator::positionChanged, handle.get(), &RotateHandle::setTargetPosition);

        connect(handle.get(), &RotateHandle::onBegin, this, [this] (double value) { handleOnBegin(ValueType::Rotate, value); });
        connect(handle.get(), &RotateHandle::onChange, this, [this, axis] (double value) { handleOnChange(ValueType::Rotate, axis, value); });
        connect(handle.get(), &RotateHandle::onEnd, this, [this] { handleOnEnd(ValueType::Rotate); });
        _rotateHandles.push_back(std::move(handle));
    }
}

glm::dvec3 ObjectManipulator::position() const {
    LATTICE_OPTIONAL_GUARD(item, _item, return glm::dvec3(0);)
    return item->location().position;
}

void ObjectManipulator::handleOnBegin(ValueType type, double value) {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    auto changeText = [&] {
        switch (type) {
        case ValueType::Translate:
            return tr("Move Item");
        case ValueType::Scale:
            return tr("Scale Item");
        case ValueType::Rotate:
            return tr("Rotate Item");
        }
    }();

    _appState->document()->history()->beginChange(changeText);
    _initialLocation = item->location();
    _initialValue = value;
}

void ObjectManipulator::handleOnChange(ValueType type, int axis, double value) {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    auto loc = _initialLocation;
    switch (type) {
    case ValueType::Translate:
        loc.position[axis] += value - _initialValue;
        break;
    case ValueType::Scale:
        loc.scale[axis] *= value / _initialValue;
        break;
    case ValueType::Rotate: {
        glm::dvec3 eulerAngles(0);
        eulerAngles[axis] = value - _initialValue;
        loc.rotation = glm::dquat(eulerAngles) * loc.rotation;
        break;
    }
    }
    item->setLocation(loc);
}

void ObjectManipulator::handleOnEnd(ValueType type) {
    Q_UNUSED(type);
}

void ObjectManipulator::connectToItem(const std::optional<SP<Document::Item> > &maybeItem) {
    disconnect(_connection);
    _item = maybeItem;
    LATTICE_OPTIONAL_GUARD(item, maybeItem, return;)
    auto itemPtr = item.get();
    _connection = connect(itemPtr, &Document::Item::locationChanged, this, [this] {
        emit positionChanged(position());
    });
    emit positionChanged(position());
}

}
} // namespace Editor
} // namespace Lattice
