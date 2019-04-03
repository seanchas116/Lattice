#pragma once

#include <QWidget>
#include "../support/Shorthands.hpp"

namespace Lattice {

namespace State {
class AppState;
}

namespace UI {

class ObjectListView final : public QWidget {
    Q_OBJECT
public:
    explicit ObjectListView(const SP<State::AppState> &appState, QWidget *parent = nullptr);

private:
    SP<State::AppState> _appState;
};

}
} // namespace Shapecraft