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
namespace Mesh {
class Vertex;
}

namespace Editor {
namespace Manipulator {

class MeshManipulator : public Manipulator {
    Q_OBJECT
public:
    MeshManipulator(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object);

private:
    void handleOnDragBegin(ValueType type, glm::dvec3 values);
    void handleOnDragMove(ValueType type, glm::dvec3 values);
    void handleOnDragEnd(ValueType type);

    void updatePosition();

    SP<State::AppState> _appState;
    SP<Document::MeshObject> _object;
    std::unordered_map<SP<Mesh::Vertex>, glm::dvec3> _initialPositions;
    glm::dvec3 _initialMedianPos;
    glm::dvec3 _initialValues;
    QMetaObject::Connection _connection;
};

}
} // namespace Editor
} // namespace Lattice
