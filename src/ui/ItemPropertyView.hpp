#pragma once

#include "../support/Shorthands.hpp"
#include "../support/Location.hpp"
#include <QWidget>
#include <array>
#include <unordered_set>

namespace Lattice::Widget {
class DoubleSpinBox;
}

namespace Lattice::Document {
class Item;
}

namespace Lattice::UI {

class ItemPropertyView final : public QWidget {
    Q_OBJECT
public:
    explicit ItemPropertyView(QWidget *parent = nullptr);

    void setItems(const std::unordered_set<SP<Document::Item>> &items);
    auto& items() const { return _items; }

private:
    enum class LocationMember {
        Position, Scale, Rotation,
    };

    void setLocation();

    void handleLocationValueChange(LocationMember member, int index, double value);

    std::unordered_set<SP<Document::Item>> _items;

    QMetaObject::Connection _itemConnection;

    Location _location;

    std::array<Widget::DoubleSpinBox*, 3> _positionSpinBoxes;
    std::array<Widget::DoubleSpinBox*, 3> _scaleSpinBoxes;
    std::array<Widget::DoubleSpinBox*, 3> _rotationSpinBoxes;
};

} // namespace Lattice
