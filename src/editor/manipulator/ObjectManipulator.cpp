#include "ObjectManipulator.hpp"
#include "../../state/AppState.hpp"
#include "../../document/Document.hpp"
#include "../../document/Item.hpp"
#include "../../document/History.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

ObjectManipulator::ObjectManipulator(const SP<State::AppState> &appState) : _appState(appState)
{
    connectToItem(appState->document()->currentItem());
    connect(appState->document().get(), &Document::Document::currentItemChanged, this, &ObjectManipulator::connectToItem);
    connect(this, &Manipulator::onBegin, this, &ObjectManipulator::handleOnBegin);
    connect(this, &Manipulator::onChange, this, &ObjectManipulator::handleOnChange);
    connect(this, &Manipulator::onEnd, this, &ObjectManipulator::handleOnEnd);
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

void ObjectManipulator::connectToItem(const Opt<SP<Document::Item> > &maybeItem) {
    disconnect(_connection);
    _item = maybeItem;
    LATTICE_OPTIONAL_GUARD(item, maybeItem, return;)
    auto itemPtr = item.get();
    _connection = connect(itemPtr, &Document::Item::locationChanged, this, [this] {
        updatePosition();
    });
    updatePosition();
}

void ObjectManipulator::updatePosition() {
    LATTICE_OPTIONAL_GUARD(item, _item, setTargetPosition(glm::dvec3(0)); return;)
    setTargetPosition(item->location().position);
}

}
} // namespace Editor
} // namespace Lattice
