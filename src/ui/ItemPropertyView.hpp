#pragma once

#include "../support/Shorthands.hpp"
#include <QWidget>
#include <array>
#include <unordered_set>

namespace Lattice {

namespace State {
class AppState;
}

namespace Widget {
class DoubleSpinBox;
}

namespace Document {
class Object;
}

namespace UI {

class ItemPropertyView final : public QWidget {
    Q_OBJECT
public:
    explicit ItemPropertyView(const SP<State::AppState> &appState, QWidget *parent = nullptr);

    void setItems(const std::unordered_set<SP<Document::Object>> &items);
    auto& items() const { return _items; }

private:
    enum class LocationMember {
        Position, Scale, Rotation,
    };

    void setLocation();

    void handleLocationValueChange(LocationMember member, int index, double value);

    SP<State::AppState> _appState;

    std::unordered_set<SP<Document::Object>> _items;
    std::vector<QMetaObject::Connection> _itemConnections;

    std::array<Widget::DoubleSpinBox*, 3> _positionSpinBoxes;
    std::array<Widget::DoubleSpinBox*, 3> _scaleSpinBoxes;
    std::array<Widget::DoubleSpinBox*, 3> _rotationSpinBoxes;
};

}
} // namespace Lattice
