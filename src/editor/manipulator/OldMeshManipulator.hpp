#pragma once
#include "Manipulator.hpp"
#include "../../support/Location.hpp"

namespace Lattice {

namespace State {
class AppState;
}
namespace Document {
class MeshObject;
}
namespace OldMesh {
class Vertex;
}

namespace Editor {
namespace Manipulator {

class OldMeshManipulator : public Manipulator {
    Q_OBJECT
public:
    OldMeshManipulator(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object);

private:
    void handleOnDragBegin(ValueType type, glm::dvec3 values);
    void handleOnDragMove(ValueType type, glm::dvec3 values);
    void handleOnDragEnd(ValueType type);

    void updatePosition();

    SP<State::AppState> _appState;
    SP<Document::MeshObject> _object;
    std::unordered_map<SP<OldMesh::Vertex>, glm::dvec3> _initialPositions;
    glm::dvec3 _initialMedianPos;
    glm::dvec3 _initialValues;
    QMetaObject::Connection _connection;
};

}
} // namespace Editor
} // namespace Lattice
