#pragma once
#include "../../support/Location.hpp"
#include "Manipulator.hpp"
#include <meshlib/Handle.hpp>

namespace meshlib {
class Mesh;
}

namespace Lattice {

namespace Editor {
namespace Manipulator {

class MeshManipulator : public Manipulator {
    Q_OBJECT
  public:
    MeshManipulator(const glm::dmat4 &objectToWorldMatrix, const SP<meshlib::Mesh> &mesh);

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
    SP<meshlib::Mesh> _mesh;
    std::unordered_map<meshlib::VertexHandle, glm::dvec3> _initialPositions;
    glm::dvec3 _initialMedianPos;
    glm::dvec3 _initialValues;
};

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
