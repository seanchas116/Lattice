#pragma once

#include <QWidget>
#include "../support/Pointer.hpp"

namespace Lattice {

class AppState;

class ItemListPane final : public QWidget {
    Q_OBJECT
public:
    explicit ItemListPane(const SP<AppState> &appState, QWidget *parent = nullptr);

private:
    SP<AppState> _appState;
};

} // namespace Shapecraft
