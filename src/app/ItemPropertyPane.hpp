#pragma once

#include <QWidget>
#include "../support/Pointer.hpp"

namespace Lattice {

class AppState;

class ItemPropertyPane : public QWidget {
    Q_OBJECT
public:
    explicit ItemPropertyPane(const SP<AppState> &appState, QWidget *parent = nullptr);

private:
    SP<AppState> _appState;
};

} // namespace Lattice
