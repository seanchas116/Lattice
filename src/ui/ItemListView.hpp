#pragma once

#include <QWidget>
#include "../support/Shorthands.hpp"

namespace Lattice::State {
class AppState;
}

namespace Lattice::UI {

class ItemListView final : public QWidget {
    Q_OBJECT
public:
    explicit ItemListView(const SP<State::AppState> &appState, QWidget *parent = nullptr);

private:
    SP<State::AppState> _appState;
};

} // namespace Shapecraft
