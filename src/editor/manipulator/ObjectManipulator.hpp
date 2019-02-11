#pragma once
#include "Manipulator.hpp"
#include "../../support/Location.hpp"

namespace Lattice {

namespace UI {
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
    ObjectManipulator(const SP<UI::AppState>& appState);

private:
    void handleOnBegin(ValueType type, double value);
    void handleOnChange(ValueType type, int axis, double value);
    void handleOnEnd(ValueType type);

    void connectToItem(const Opt<SP<Document::Item>> &maybeItem);

    void updatePosition();

    SP<UI::AppState> _appState;
    Opt<SP<Document::Item>> _item;
    double _initialValue;
    Location _initialLocation;
    QMetaObject::Connection _connection;
};

}
} // namespace Editor
} // namespace Lattice
