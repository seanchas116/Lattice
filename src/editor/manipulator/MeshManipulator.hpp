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
    MeshManipulator(const SP<State::AppState>& appState);

private:
    void handleOnBegin(ValueType type, double value);
    void handleOnChange(ValueType type, int axis, double value);
    void handleOnEnd(ValueType type);

    void setItem(const Opt<SP<Document::MeshItem>> &maybeItem);

    void updatePosition();

    SP<State::AppState> _appState;
    Opt<SP<Document::MeshItem>> _item;
    std::unordered_map<SP<Mesh::Vertex>, glm::dvec3> _initialPositions;
    glm::dvec3 _initialMedianPos;
    double _initialValue;
    QMetaObject::Connection _connection;
};

}
} // namespace Editor
} // namespace Lattice
