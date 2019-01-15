#pragma once
#include "Manipulator.hpp"
#include "../../support/Location.hpp"

namespace Lattice {

namespace UI {
class AppState;
}
namespace Document {
class MeshItem;
}

namespace Editor {
namespace Manipulator {

class MeshManipulator : public Manipulator {
    Q_OBJECT
public:
    MeshManipulator(const SP<UI::AppState>& appState);

private:
    void handleOnBegin(ValueType type, double value);
    void handleOnChange(ValueType type, int axis, double value);
    void handleOnEnd(ValueType type);

    void connectToItem(const std::optional<SP<Document::MeshItem>> &maybeItem);

    void updatePosition();

    SP<UI::AppState> _appState;
    std::optional<SP<Document::MeshItem>> _item;
    double _initialValue;
    Location _initialLocation;
    QMetaObject::Connection _connection;
};

}
} // namespace Editor
} // namespace Lattice
