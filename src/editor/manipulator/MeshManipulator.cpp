#include "MeshManipulator.hpp"
#include "../../mesh/Mesh.hpp"

using namespace glm;

namespace Lattice {
namespace Editor {
namespace Manipulator {

namespace {

template <typename TPositions>
glm::vec3 medianPosition(const TPositions& positions) {
    if (positions.empty()) {
        return vec3(0);
    }

    glm::vec3 minPos(std::numeric_limits<double>::infinity());
    glm::vec3 maxPos(-std::numeric_limits<double>::infinity());

    for (auto p : positions) {
        minPos = min(minPos, p);
        maxPos = max(maxPos, p);
    }

    return (minPos + maxPos) * 0.5f;
}

}

MeshManipulator::MeshManipulator(const glm::dmat4& objectToWorldMatrix, const SP<Mesh::Mesh> &mesh) : _objectToWorld(objectToWorldMatrix),
                                                                                                      _worldToObject(inverse(objectToWorldMatrix)),
                                                                                                      _mesh(mesh) {
    updatePosition();

    connect(this, &Manipulator::onDragBegin, this, &MeshManipulator::handleOnDragBegin);
    connect(this, &Manipulator::onDragMove, this, &MeshManipulator::handleOnDragMove);
    connect(this, &Manipulator::onDragEnd, this, &MeshManipulator::handleOnDragEnd);
}

void MeshManipulator::handleOnDragBegin(ValueType type, dvec3 values) {
    Q_UNUSED(type);
    _initialValues = values;

    auto& mesh = *_mesh;

    auto selectedVertices = mesh.selectedVertices() | ranges::to_vector;
    auto medianPos = medianPosition(selectedVertices | ranges::view::transform([&](auto v) { return mesh.position(v); }));

    for (auto v : selectedVertices) {
        _initialPositions[v] = (_objectToWorld * dvec4(mesh.position(v), 1)).xyz;
    }

    _initialMedianPos = (_objectToWorld * dvec4(medianPos, 1)).xyz;
}

void MeshManipulator::handleOnDragMove(ValueType type, dvec3 values) {
    auto& mesh = _object->oldMesh();
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
