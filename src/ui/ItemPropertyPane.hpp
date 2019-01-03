#pragma once

#include "../support/Pointer.hpp"
#include <QWidget>
#include <array>

class QDoubleSpinBox;

namespace Lattice::UI {

class AppState;

class ItemPropertyPane final : public QWidget {
    Q_OBJECT
public:
    explicit ItemPropertyPane(const SP<AppState> &appState, QWidget *parent = nullptr);

private:
    void onCurrentItemChanged();
    void onLocationChanged();

    void setLocation();

    SP<AppState> _appState;
    QMetaObject::Connection _itemConnection;

    std::array<QDoubleSpinBox*, 3> _positionSpinBoxes;
    std::array<QDoubleSpinBox*, 3> _scaleSpinBoxes;
    std::array<QDoubleSpinBox*, 3> _rotationSpinBoxes;
};

} // namespace Lattice
