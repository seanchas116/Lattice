#pragma once

#include <QWidget>
#include "../support/Pointer.hpp"

class QDoubleSpinBox;

namespace Lattice {

class AppState;

class ItemPropertyPane : public QWidget {
    Q_OBJECT
public:
    explicit ItemPropertyPane(const SP<AppState> &appState, QWidget *parent = nullptr);

private:
    void onSelectedItemChanged();
    void onLocationChanged();

    SP<AppState> _appState;
    std::vector<QMetaObject::Connection> _itemConnections;

    QDoubleSpinBox* _posXSpinBox;
    QDoubleSpinBox* _posYSpinBox;
    QDoubleSpinBox* _posZSpinBox;
};

} // namespace Lattice
