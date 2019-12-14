#pragma once
#include "../../support/Location.hpp"
#include "Manipulator.hpp"
#include <meshlib/Handle.hpp>

namespace Lattice {

namespace Mesh {
class Mesh;
}

namespace Editor {
namespace Manipulator {

class MeshManipulator : public Manipulator {
    Q_OBJECT
  public:
    MeshManipulator(const glm::dmat4 &objectToWorldMatrix, const SP<Mesh::Mesh> &mesh);

    void updatePosition();

  signals:
    void meshChanged();
    void meshChangeFinished();

  private:
    void handleOnDragBegin(ValueType type, glm::dvec3 values);
    void handleOnDragMove(ValueType type, glm::dvec3 values);
    void handleOnDragEnd(ValueType type);

    glm::dmat4 _objectToWorld;
    glm::dmat4 _worldToObject;
    SP<Mesh::Mesh> _mesh;
    std::unordered_map<Mesh::VertexHandle, glm::dvec3> _initialPositions;
    glm::dvec3 _initialMedianPos;
    glm::dvec3 _initialValues;
};

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
