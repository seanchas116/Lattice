#pragma once
#include "Manipulator.hpp"
#include "../../support/Location.hpp"

namespace Lattice {

namespace State {
class AppState;
}
namespace Document {
class Item;
}

namespace Editor {
namespace Manipulator {

class ObjectManipulator : public Manipulator {
    Q_OBJECT
public:
    ObjectManipulator(const SP<State::AppState>& appState);

private:
    void handleOnDragBegin(ValueType type, glm::dvec3 values);
    void handleOnDragMove(ValueType type, glm::dvec3 values);
    void handleOnDragEnd(ValueType type);

    void setItems(const std::unordered_set<SP<Document::Item>> &items);

    void updatePosition();

    SP<State::AppState> _appState;
    std::unordered_set<SP<Document::Item>> _items;
    glm::dvec3 _initialValues;
    std::unordered_map<SP<Document::Item>, Location> _initialLocations;
    std::vector<QMetaObject::Connection> _connections;
};

}
} // namespace Editor
} // namespace Lattice
