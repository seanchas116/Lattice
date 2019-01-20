#include "MeshManipulator.hpp"
#include "../../ui/AppState.hpp"
#include "../../document/Document.hpp"
#include "../../document/MeshItem.hpp"
#include "../../document/History.hpp"

using namespace glm;

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

    _appState->document()->history()->beginChange(tr("Move Vertex"));

    _initialValue = value;

    for (auto& v : _appState->document()->meshSelection().vertices) {
        _initialPositions[v] = v->position();
    }
    _initialMedianPos = _appState->document()->meshSelection().medianPosition();
}

void MeshManipulator::handleOnChange(ValueType type, int axis, double value) {
    // TODO
    LATTICE_OPTIONAL_GUARD(item, _item, return;)
    auto& mesh = item->mesh();

    switch (type) {
    case ValueType::Translate: {
        dvec3 offset(0);
        offset[axis] = value - _initialValue;
        for (auto& [vertex, initialPos] : _initialPositions) {
            mesh->setPosition(vertex, initialPos + offset);
        }
        break;
    }
    case ValueType::Scale: {
        dvec3 ratio(1);
        ratio[axis] = value / _initialValue;
        for (auto& [vertex, initialPos] : _initialPositions) {
            dvec3 initialOffset = initialPos - _initialMedianPos;
            mesh->setPosition(vertex, _initialMedianPos + initialOffset * ratio);
        }
        break;
    }
    case ValueType::Rotate: {
        dvec3 eulerAngles(0);
        eulerAngles[axis] = value - _initialValue;
        auto matrix = mat4_cast(dquat(eulerAngles));

        for (auto& [vertex, initialPos] : _initialPositions) {
            dvec3 initialOffset = initialPos - _initialMedianPos;
            dvec3 offset = (matrix * dvec4(initialOffset, 0)).xyz;
            mesh->setPosition(vertex, _initialMedianPos + offset);
        }
        break;
    }
    default:
        // TODO
        break;
    }
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
    _connection = connect(item->mesh().get(), &Document::Mesh::changed, this, [this] {
        updatePosition();
    });
    updatePosition();
}

void MeshManipulator::updatePosition() {
    LATTICE_OPTIONAL_GUARD(item, _item, setTargetPosition(dvec3(0));)
    auto median = _appState->document()->meshSelection().medianPosition();
    auto matrix = item->location().matrixToWorld();
    setTargetPosition((matrix * dvec4(median, 1)).xyz);
}

}
} // namespace Editor
} // namespace Lattice
