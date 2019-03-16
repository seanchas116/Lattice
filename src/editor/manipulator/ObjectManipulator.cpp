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
    setItems(appState->document()->selectedItems());
    connect(appState->document().get(), &Document::Document::selectedItemsChanged, this, &ObjectManipulator::setItems);
    connect(this, &Manipulator::onBegin, this, &ObjectManipulator::handleOnBegin);
    connect(this, &Manipulator::onChange, this, &ObjectManipulator::handleOnChange);
    connect(this, &Manipulator::onEnd, this, &ObjectManipulator::handleOnEnd);
}

void ObjectManipulator::handleOnBegin(ValueType type, double value) {
    if (_items.empty()) {
        return;
    }
    auto item = *_items.begin();

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
    if (_items.empty()) {
        return;
    }
    auto item = *_items.begin();

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

void ObjectManipulator::setItems(const std::unordered_set<SP<Document::Item> > &items) {
    for (auto& c : _connections) {
        disconnect(c);
    }
    _connections.clear();

    _items = items;

    for (auto& item : items) {
        auto c = connect(item.get(), &Document::Item::locationChanged, this, [this] {
            updatePosition();
        });
        _connections.push_back(c);
    }
    updatePosition();
}

void ObjectManipulator::updatePosition() {
    if (_items.empty()) {
        setTargetPosition(glm::dvec3(0));
        return;
    }

    glm::dvec3 minPos(INFINITY);
    glm::dvec3 maxPos(-INFINITY);

    for (auto& item : _items) {
        auto p = item->location().position;
        minPos = glm::min(minPos, p);
        maxPos = glm::max(maxPos, p);
    }

    auto position = (minPos + maxPos) * 0.5;
    setTargetPosition(position);
}

}
} // namespace Editor
} // namespace Lattice
