#pragma once
#include "../../support/Location.hpp"
#include "Manipulator.hpp"
#include <unordered_map>
#include <unordered_set>

namespace Lattice {

namespace State {
class AppState;
}
namespace Document {
class Object;
}

namespace Editor {
namespace Manipulator {

class ObjectManipulator : public Manipulator {
    Q_OBJECT
  public:
    ObjectManipulator(const SP<State::AppState> &appState);

  private:
    void handleOnDragBegin(ValueType type, glm::dvec3 values);
    void handleOnDragMove(ValueType type, glm::dvec3 values);
    void handleOnDragEnd(ValueType type);

    void setObjects(const std::unordered_set<SP<Document::Object>> &objects);

    void updatePosition();

    SP<State::AppState> _appState;
    std::unordered_set<SP<Document::Object>> _objects;
    glm::dvec3 _initialValues;
    std::unordered_map<SP<Document::Object>, Location> _initialLocations;
    std::vector<QMetaObject::Connection> _connections;
};

} // namespace Manipulator
} // namespace Editor
} // namespace Lattice
