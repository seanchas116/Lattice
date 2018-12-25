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
    void onCurrentItemChanged();
    void onLocationChanged();

    void setLocation();

    SP<AppState> _appState;
    QMetaObject::Connection _itemConnection;

    QDoubleSpinBox* _posXSpinBox;
    QDoubleSpinBox* _posYSpinBox;
    QDoubleSpinBox* _posZSpinBox;
};

} // namespace Lattice
