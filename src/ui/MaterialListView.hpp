#pragma once

#include <QWidget>
#include "../support/Shorthands.hpp"

namespace Lattice::UI {

class AppState;

class MaterialListView final : public QWidget {
    Q_OBJECT
public:
    explicit MaterialListView(const SP<AppState>& appState, QWidget *parent = nullptr);

private:
    SP<AppState> _appState;
};

} // namespace Lattice
