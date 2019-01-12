#include "Controller.hpp"
#include "TranslateHandle.hpp"
#include "ScaleHandle.hpp"
#include "RotateHandle.hpp"
#include "../../ui/AppState.hpp"
#include "../../document/Document.hpp"
#include "../../document/Item.hpp"
#include "../../document/History.hpp"
#include "../../support/OptionalGuard.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

Controller::Controller(const SP<UI::AppState> &appState) : _appState(appState)
{
    connectToItem(appState->document()->currentItem());
    connect(appState->document().get(), &Document::Document::currentItemChanged, this, &Controller::connectToItem);

    for (int axis = 0; axis < 3; ++axis) {
        auto handle = makeShared<TranslateHandle>(axis, TranslateHandle::HandleType::Translate);
        handle->setTargetPosition(position());
        connect(this, &Controller::positionChanged, handle.get(), &TranslateHandle::setTargetPosition);

        connect(handle.get(), &TranslateHandle::translateStarted, this, [this] { onBegin(ValueType::Translate); });
        connect(handle.get(), &TranslateHandle::translateChanged, this, [this, axis] (double offset) { onChange(ValueType::Translate, axis, offset); });
        connect(handle.get(), &TranslateHandle::translateFinished, this, [this] { onEnd(ValueType::Translate); });
        _translateHandles.push_back(std::move(handle));
    }

    for (int axis = 0; axis < 3; ++axis) {
        auto handle = makeShared<ScaleHandle>(axis);
        handle->setTargetPosition(position());
        connect(this, &Controller::positionChanged, handle.get(), &ScaleHandle::setTargetPosition);

        connect(handle.get(), &ScaleHandle::scaleStarted, this, [this] { onBegin(ValueType::Scale); });
        connect(handle.get(), &ScaleHandle::scaleChanged, this, [this, axis] (double offset) { onChange(ValueType::Scale, axis, offset); });
        connect(handle.get(), &ScaleHandle::scaleFinished, this, [this] { onEnd(ValueType::Scale); });
        _scaleHandles.push_back(std::move(handle));
    }

    for (int axis = 0; axis < 3; ++axis) {
        auto handle = makeShared<RotateHandle>(axis);
        handle->setTargetPosition(position());
        connect(this, &Controller::positionChanged, handle.get(), &RotateHandle::setTargetPosition);

        connect(handle.get(), &RotateHandle::rotateStarted, this, [this] { onBegin(ValueType::Rotate); });
        connect(handle.get(), &RotateHandle::rotateChanged, this, [this, axis] (double offset) { onChange(ValueType::Rotate, axis, offset); });
        connect(handle.get(), &RotateHandle::rotateFinished, this, [this] { onEnd(ValueType::Rotate); });
        _rotateHandles.push_back(std::move(handle));
    }
}

glm::dvec3 Controller::position() const {
    LATTICE_OPTIONAL_GUARD(item, _item, return glm::dvec3(0);)
    return item->location().position;
}

void Controller::onBegin(ValueType type) {
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
}

void Controller::onChange(ValueType type, int axis, double offset) {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    auto loc = _initialLocation;
    switch (type) {
    case ValueType::Translate:
        loc.position[axis] += offset;
        break;
    case ValueType::Scale:
        loc.scale[axis] *= offset;
        break;
    case ValueType::Rotate: {
        glm::dvec3 eulerAngles(0);
        eulerAngles[axis] = offset;
        loc.rotation = glm::dquat(eulerAngles) * loc.rotation;
        break;
    }
    }
    item->setLocation(loc);
}

void Controller::onEnd(ValueType type) {
    Q_UNUSED(type);
}

void Controller::connectToItem(const std::optional<SP<Document::Item> > &maybeItem) {
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
