#pragma once

#include "../support/Shorthands.hpp"
#include "../support/Location.hpp"
#include <QWidget>
#include <array>

class QDoubleSpinBox;

namespace Lattice::State {
class AppState;
}

namespace Lattice::UI {

class ItemPropertyView final : public QWidget {
    Q_OBJECT
public:
    explicit ItemPropertyView(const SP<State::AppState> &appState, QWidget *parent = nullptr);

private:
    void onCurrentItemChanged();
    void onLocationChanged();

    void setLocation();

    void setEnabled(bool enabled);

    SP<State::AppState> _appState;
    QMetaObject::Connection _itemConnection;

    Location _location;

    std::array<QDoubleSpinBox*, 3> _positionSpinBoxes;
    std::array<QDoubleSpinBox*, 3> _scaleSpinBoxes;
    std::array<QDoubleSpinBox*, 3> _rotationSpinBoxes;
};

} // namespace Lattice
