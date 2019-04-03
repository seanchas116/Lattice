#pragma once
#include "../support/Shorthands.hpp"
#include <QWidget>
#include <unordered_set>

namespace Lattice {

namespace State {
class AppState;
}
namespace Document {
class Object;
}

namespace UI {

class ObjectPropertyView;
class MeshPropertyView;

class PropertyView : public QWidget {
    Q_OBJECT
public:
    explicit PropertyView(const SP<State::AppState> &appState, QWidget *parent = nullptr);

private:
    SP<State::AppState> _appState;
};

} // namespace UI
} // namespace Lattice
