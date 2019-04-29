#include "MeshManipulator.hpp"
#include "../../state/AppState.hpp"
#include "../../document/Document.hpp"
#include "../../document/MeshObject.hpp"
#include "../../document/History.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace Manipulator {

MeshManipulator::MeshManipulator(const SP<State::AppState> &appState, const SP<Document::MeshObject> &object) :
    _appState(appState),
    _object(object)
{
    connect(object->mesh().get(), &OldMesh::Mesh::changed, this, &MeshManipulator::updatePosition);
    connect(appState->document().get(), &Document::Document::meshSelectionChanged, this, &MeshManipulator::updatePosition);
    updatePosition();

    connect(this, &Manipulator::onDragBegin, this, &MeshManipulator::handleOnDragBegin);
    connect(this, &Manipulator::onDragMove, this, &MeshManipulator::handleOnDragMove);
    connect(this, &Manipulator::onDragEnd, this, &MeshManipulator::handleOnDragEnd);
}

void MeshManipulator::handleOnDragBegin(ValueType type, dvec3 values) {
    Q_UNUSED(type);
    _appState->document()->history()->beginChange(tr("Move Vertex"));

    _initialValues = values;

    dmat4 objectToWorld = _object->location().matrixToWorld();

    for (auto& v : _appState->document()->meshSelection().vertices) {
        _initialPositions[v] = (objectToWorld * dvec4(v->position(), 1)).xyz;
    }
    _initialMedianPos = (objectToWorld * dvec4(_appState->document()->meshSelection().medianPosition(), 1)).xyz;
}

void MeshManipulator::handleOnDragMove(ValueType type, dvec3 values) {
    auto& mesh = _object->mesh();
    auto worldToObject = _object->location().matrixToModel();

    switch (type) {
    case ValueType::Translate: {
        dvec3 offset = values - _initialValues;
        std::unordered_map<SP<OldMesh::Vertex>, dvec3> positions;
        for (auto& [vertex, initialPos] : _initialPositions) {
            auto newPos = initialPos + offset;
            positions[vertex] = (worldToObject * dvec4(newPos, 1)).xyz;
        }
        mesh->setPosition(positions);
        break;
    }
    case ValueType::Scale: {
        dvec3 ratio = values / _initialValues;
        std::unordered_map<SP<OldMesh::Vertex>, dvec3> positions;
        for (auto& [vertex, initialPos] : _initialPositions) {
            dvec3 initialOffset = initialPos - _initialMedianPos;
            auto newPos = _initialMedianPos + initialOffset * ratio;
            positions[vertex] = (worldToObject * dvec4(newPos, 1)).xyz;
        }
        mesh->setPosition(positions);
        break;
    }
    case ValueType::Rotate: {
        dvec3 eulerAngles = values - _initialValues;
        auto matrix = mat4_cast(dquat(eulerAngles));

        std::unordered_map<SP<OldMesh::Vertex>, dvec3> positions;
        for (auto& [vertex, initialPos] : _initialPositions) {
            dvec3 initialOffset = initialPos - _initialMedianPos;
            dvec3 offset = (matrix * dvec4(initialOffset, 0)).xyz;
            auto newPos = _initialMedianPos + offset;
            positions[vertex] = (worldToObject * dvec4(newPos, 1)).xyz;
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
    auto matrix = _object->location().matrixToWorld();
    setTargetPosition((matrix * dvec4(median, 1)).xyz);
}

}
} // namespace Editor
} // namespace Lattice
