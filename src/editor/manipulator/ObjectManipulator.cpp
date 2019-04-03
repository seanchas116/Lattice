#include "ObjectManipulator.hpp"
#include "../../state/AppState.hpp"
#include "../../document/Document.hpp"
#include "../../document/Object.hpp"
#include "../../document/History.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

ObjectManipulator::ObjectManipulator(const SP<State::AppState> &appState) : _appState(appState)
{
    setItems(appState->document()->selectedItems());
    connect(appState->document().get(), &Document::Document::selectedItemsChanged, this, &ObjectManipulator::setItems);
    connect(this, &Manipulator::onDragBegin, this, &ObjectManipulator::handleOnDragBegin);
    connect(this, &Manipulator::onDragMove, this, &ObjectManipulator::handleOnDragMove);
    connect(this, &Manipulator::onDragEnd, this, &ObjectManipulator::handleOnDragEnd);
}

void ObjectManipulator::handleOnDragBegin(ValueType type, glm::dvec3 values) {
    if (_items.empty()) {
        return;
    }

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
    for (auto& item : _items) {
        _initialLocations[item] = item->location();
    }
    _initialValues = values;
}

void ObjectManipulator::handleOnDragMove(ValueType type, glm::dvec3 values) {
    // TODO: scale and rotate from median center

    if (_items.empty()) {
        return;
    }
    for (auto& item : _items) {
        auto loc = _initialLocations.at(item);
        switch (type) {
        case ValueType::Translate:
            loc.position += values - _initialValues;
            break;
        case ValueType::Scale:
            loc.scale *= values / _initialValues;
            break;
        case ValueType::Rotate: {
            glm::dvec3 eulerAngles = values - _initialValues;
            loc.rotation = glm::dquat(eulerAngles) * loc.rotation;
            break;
        }
        }
        item->setLocation(loc);
    }
}

void ObjectManipulator::handleOnDragEnd(ValueType type) {
    Q_UNUSED(type);
    _initialLocations.clear();
}

void ObjectManipulator::setItems(const std::unordered_set<SP<Document::Object> > &items) {
    for (auto& c : _connections) {
        disconnect(c);
    }
    _connections.clear();

    _items = items;

    for (auto& item : items) {
        auto c = connect(item.get(), &Document::Object::locationChanged, this, [this] {
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

    glm::dvec3 minPos(std::numeric_limits<double>::infinity());
    glm::dvec3 maxPos(-std::numeric_limits<double>::infinity());

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
