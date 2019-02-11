#pragma once

#include <QWidget>
#include "../support/Shorthands.hpp"

namespace Lattice::UI {

class AppState;

class ItemListView final : public QWidget {
    Q_OBJECT
public:
    explicit ItemListView(const SP<AppState> &appState, QWidget *parent = nullptr);

private:
    SP<AppState> _appState;
};

} // namespace Shapecraft
