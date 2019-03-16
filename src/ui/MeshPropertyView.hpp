#pragma once

#include "../support/Shorthands.hpp"
#include <QWidget>

namespace Lattice {

namespace State {
class AppState;
}

namespace UI {

class MeshPropertyView : public QWidget {
    Q_OBJECT
public:
    explicit MeshPropertyView(const SP<State::AppState> &appState, QWidget *parent = nullptr);

private:
    SP<State::AppState> _appState;
};

} // namespace UI
} // namespace Lattice
