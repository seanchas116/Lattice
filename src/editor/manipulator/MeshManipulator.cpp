#include "MeshManipulator.hpp"
#include "../../state/AppState.hpp"
#include "../../document/Document.hpp"
#include "../../document/MeshItem.hpp"
#include "../../document/History.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace Manipulator {

MeshManipulator::MeshManipulator(const SP<State::AppState> &appState, const SP<Document::MeshItem> &item) :
    _appState(appState),
    _item(item)
{
    connect(item->mesh().get(), &Mesh::Mesh::changed, this, &MeshManipulator::updatePosition);
    connect(appState->document().get(), &Document::Document::meshSelectionChanged, this, &MeshManipulator::updatePosition);
    updatePosition();

    connect(this, &Manipulator::onDragBegin, this, &MeshManipulator::handleOnDragBegin);
    connect(this, &Manipulator::onDragMove, this, &MeshManipulator::handleOnDragMove);
    connect(this, &Manipulator::onDragEnd, this, &MeshManipulator::handleOnDragEnd);
}

void MeshManipulator::handleOnDragBegin(ValueType type, double value) {
    _appState->document()->history()->beginChange(tr("Move Vertex"));

    _initialValue = value;

    dmat4 itemToWorld = _item->location().matrixToWorld();

    for (auto& v : _appState->document()->meshSelection().vertices) {
        _initialPositions[v] = (itemToWorld * dvec4(v->position(), 1)).xyz;
    }
    _initialMedianPos = (itemToWorld * dvec4(_appState->document()->meshSelection().medianPosition(), 1)).xyz;
}

void MeshManipulator::handleOnDragMove(ValueType type, int axis, double value) {
    auto& mesh = _item->mesh();
    auto worldToItem = _item->location().matrixToModel();

    switch (type) {
    case ValueType::Translate: {
        dvec3 offset(0);
        offset[axis] = value - _initialValue;
        std::unordered_map<SP<Mesh::Vertex>, dvec3> positions;
        for (auto& [vertex, initialPos] : _initialPositions) {
            auto newPos = initialPos + offset;
            positions[vertex] = (worldToItem * dvec4(newPos, 1)).xyz;
        }
        mesh->setPosition(positions);
        break;
    }
    case ValueType::Scale: {
        dvec3 ratio(1);
        ratio[axis] = value / _initialValue;
        std::unordered_map<SP<Mesh::Vertex>, dvec3> positions;
        for (auto& [vertex, initialPos] : _initialPositions) {
            dvec3 initialOffset = initialPos - _initialMedianPos;
            auto newPos = _initialMedianPos + initialOffset * ratio;
            positions[vertex] = (worldToItem * dvec4(newPos, 1)).xyz;
        }
        mesh->setPosition(positions);
        break;
    }
    case ValueType::Rotate: {
        dvec3 eulerAngles(0);
        eulerAngles[axis] = value - _initialValue;
        auto matrix = mat4_cast(dquat(eulerAngles));

        std::unordered_map<SP<Mesh::Vertex>, dvec3> positions;
        for (auto& [vertex, initialPos] : _initialPositions) {
            dvec3 initialOffset = initialPos - _initialMedianPos;
            dvec3 offset = (matrix * dvec4(initialOffset, 0)).xyz;
            auto newPos = _initialMedianPos + offset;
            positions[vertex] = (worldToItem * dvec4(newPos, 1)).xyz;
        }
        mesh->setPosition(positions);
        break;
    }
    }
}

void MeshManipulator::handleOnDragEnd(ValueType type) {
    Q_UNUSED(type);
    _initialPositions.clear();
}

void MeshManipulator::updatePosition() {
    auto median = _appState->document()->meshSelection().medianPosition();
    auto matrix = _item->location().matrixToWorld();
    setTargetPosition((matrix * dvec4(median, 1)).xyz);
}

}
} // namespace Editor
} // namespace Lattice
