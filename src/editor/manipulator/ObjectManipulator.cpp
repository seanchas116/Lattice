#include "ObjectManipulator.hpp"
#include "../../document/Document.hpp"
#include "../../document/History.hpp"
#include "../../document/Object.hpp"
#include "../../state/AppState.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

ObjectManipulator::ObjectManipulator(const SP<State::AppState> &appState) : _appState(appState) {
    setObjects(appState->document()->selectedObjects());
    connect(appState->document().get(), &Document::Document::selectedObjectsChanged, this, &ObjectManipulator::setObjects);
    connect(this, &Manipulator::onDragBegin, this, &ObjectManipulator::handleOnDragBegin);
    connect(this, &Manipulator::onDragMove, this, &ObjectManipulator::handleOnDragMove);
    connect(this, &Manipulator::onDragEnd, this, &ObjectManipulator::handleOnDragEnd);
}

void ObjectManipulator::handleOnDragBegin(ValueType type, glm::dvec3 values) {
    if (_objects.empty()) {
        return;
    }

    auto changeText = [&] {
        switch (type) {
        case ValueType::Translate:
            return tr("Move Object");
        case ValueType::Scale:
            return tr("Scale Object");
        case ValueType::Rotate:
            return tr("Rotate Object");
        }
    }();

    _appState->document()->history()->beginChange(changeText);
    for (auto &object : _objects) {
        _initialLocations[object] = object->location();
    }
    _initialValues = values;
}

void ObjectManipulator::handleOnDragMove(ValueType type, glm::dvec3 values) {
    // TODO: scale and rotate from median center

    if (_objects.empty()) {
        return;
    }
    for (auto &object : _objects) {
        auto loc = _initialLocations.at(object);
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
        object->setLocation(loc);
    }
}

void ObjectManipulator::handleOnDragEnd(ValueType type) {
    Q_UNUSED(type);
    _initialLocations.clear();
}

void ObjectManipulator::setObjects(const std::unordered_set<SP<Document::Object>> &objects) {
    for (auto &c : _connections) {
        disconnect(c);
    }
    _connections.clear();

    _objects = objects;

    for (auto &object : objects) {
        auto c = connect(object.get(), &Document::Object::locationChanged, this, [this] {
            updatePosition();
        });
        _connections.push_back(c);
    }
    updatePosition();
}

void ObjectManipulator::updatePosition() {
    if (_objects.empty()) {
        setTargetPosition(glm::dvec3(0));
        return;
    }

    glm::dvec3 minPos(std::numeric_limits<double>::infinity());
    glm::dvec3 maxPos(-std::numeric_limits<double>::infinity());

    for (auto &object : _objects) {
        auto p = object->location().position;
        minPos = glm::min(minPos, p);
        maxPos = glm::max(maxPos, p);
    }

    auto position = (minPos + maxPos) * 0.5;
    setTargetPosition(position);
}

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
