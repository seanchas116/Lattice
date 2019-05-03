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
    auto& mesh = *_mesh;

    switch (type) {
    case ValueType::Translate: {
        dvec3 offset = values - _initialValues;
        for (auto& [vertex, initialPos] : _initialPositions) {
            auto newPos = initialPos + offset;
            mesh.setPosition(vertex, dvec3((_worldToObject * dvec4(newPos, 1)).xyz));
        }
        break;
    }
    case ValueType::Scale: {
        dvec3 ratio = values / _initialValues;
        for (auto& [vertex, initialPos] : _initialPositions) {
            dvec3 initialOffset = initialPos - _initialMedianPos;
            auto newPos = _initialMedianPos + initialOffset * ratio;
            mesh.setPosition(vertex, dvec3((_worldToObject * dvec4(newPos, 1)).xyz));
        }
        break;
    }
    case ValueType::Rotate: {
        dvec3 eulerAngles = values - _initialValues;
        auto matrix = mat4_cast(dquat(eulerAngles));

        for (auto& [vertex, initialPos] : _initialPositions) {
            dvec3 initialOffset = initialPos - _initialMedianPos;
            dvec3 offset = (matrix * dvec4(initialOffset, 0)).xyz;
            auto newPos = _initialMedianPos + offset;
            mesh.setPosition(vertex, dvec3((_worldToObject * dvec4(newPos, 1)).xyz));
        }
        break;
    }
    }

    emit meshChanged();
}

void MeshManipulator::handleOnDragEnd(ValueType type) {
    Q_UNUSED(type);
    _initialPositions.clear();

    emit meshChangeFinished();
}

void MeshManipulator::updatePosition() {
    auto& mesh = *_mesh;
    auto selectedVertices = mesh.selectedVertices() | ranges::to_vector;
    auto medianPos = medianPosition(selectedVertices | ranges::view::transform([&](auto v) { return mesh.position(v); }));
    setTargetPosition((_objectToWorld * dvec4(medianPos, 1)).xyz);
}

}
} // namespace Editor
} // namespace Lattice
