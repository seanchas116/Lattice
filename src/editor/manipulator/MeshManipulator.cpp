#include "MeshManipulator.hpp"
#include "../../ui/AppState.hpp"
#include "../../document/Document.hpp"
#include "../../document/MeshItem.hpp"
#include "../../document/History.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

MeshManipulator::MeshManipulator(const SP<UI::AppState> &appState) : _appState(appState)
{
    connectToItem(appState->document()->editedItem());
    connect(appState->document().get(), &Document::Document::editedItemChanged, this, &MeshManipulator::connectToItem);
    connect(appState->document().get(), &Document::Document::meshSelectionChanged, this, &MeshManipulator::updatePosition);
    connect(this, &Manipulator::onBegin, this, &MeshManipulator::handleOnBegin);
    connect(this, &Manipulator::onChange, this, &MeshManipulator::handleOnChange);
    connect(this, &Manipulator::onEnd, this, &MeshManipulator::handleOnEnd);
}

void MeshManipulator::handleOnBegin(ValueType type, double value) {
    Q_UNUSED(type) // TODO
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    _initialValue = value;

    for (auto& v : _appState->document()->meshSelection().vertices) {
        _initialPositions[v] = v->position();
    }
    _initialMedianPos = _appState->document()->meshSelection().medianPosition();
}

void MeshManipulator::handleOnChange(ValueType type, int axis, double value) {
    // TODO
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    switch (type) {
    case ValueType::Translate: {
        glm::dvec3 offset(0);
        offset[axis] = value - _initialValue;
        for (auto& [vertex, initialPos] : _initialPositions) {
            vertex->setPosition(initialPos + offset);
        }
        break;
    }
    case ValueType::Scale: {
        glm::dvec3 ratio(1);
        ratio[axis] = value / _initialValue;
        for (auto& [vertex, initialPos] : _initialPositions) {
            glm::dvec3 initialOffset = initialPos - _initialMedianPos;
            vertex->setPosition(_initialMedianPos + initialOffset * ratio);
        }
        break;
    }
    case ValueType::Rotate: {
        glm::dvec3 eulerAngles(0);
        eulerAngles[axis] = value - _initialValue;
        auto matrix = glm::mat4_cast(glm::dquat(eulerAngles));

        for (auto& [vertex, initialPos] : _initialPositions) {
            glm::dvec3 initialOffset = initialPos - _initialMedianPos;
            glm::dvec3 offset = (matrix * glm::dvec4(initialOffset, 0)).xyz;
            vertex->setPosition(_initialMedianPos + offset);
        }
        break;
    }
    default:
        // TODO
        break;
    }

    item->emitMeshChanged();
}

void MeshManipulator::handleOnEnd(ValueType type) {
    Q_UNUSED(type);
    _initialPositions.clear();
}

void MeshManipulator::connectToItem(const std::optional<SP<Document::MeshItem> > &maybeItem) {
    disconnect(_connection);
    _item = maybeItem;
    LATTICE_OPTIONAL_GUARD(item, maybeItem, return;)
    auto itemPtr = item.get();
    _connection = connect(itemPtr, &Document::MeshItem::meshChanged, this, [this] {
        updatePosition();
    });
    updatePosition();
}

void MeshManipulator::updatePosition() {
    setTargetPosition(_appState->document()->meshSelection().medianPosition());
}

}
} // namespace Editor
} // namespace Lattice
