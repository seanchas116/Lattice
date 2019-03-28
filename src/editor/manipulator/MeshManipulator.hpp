#pragma once
#include "Manipulator.hpp"
#include "../../support/Location.hpp"

namespace Lattice {

namespace State {
class AppState;
}
namespace Document {
class MeshItem;
}
namespace Mesh {
class Vertex;
}

namespace Editor {
namespace Manipulator {

class MeshManipulator : public Manipulator {
    Q_OBJECT
public:
    MeshManipulator(const SP<State::AppState>& appState, const SP<Document::MeshItem>& item);

private:
    void handleOnBegin(ValueType type, double value);
    void handleOnChange(ValueType type, int axis, double value);
    void handleOnEnd(ValueType type);

    void updatePosition();

    SP<State::AppState> _appState;
    SP<Document::MeshItem> _item;
    std::unordered_map<SP<Mesh::Vertex>, glm::dvec3> _initialPositions;
    glm::dvec3 _initialMedianPos;
    double _initialValue;
    QMetaObject::Connection _connection;
};

}
} // namespace Editor
} // namespace Lattice
