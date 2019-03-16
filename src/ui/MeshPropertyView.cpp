#include "MeshPropertyView.hpp"

namespace Lattice {
namespace UI {

MeshPropertyView::MeshPropertyView(const SP<State::AppState> &appState, QWidget *parent) :
    QWidget(parent),
    _appState(appState)
{
}

} // namespace UI
} // namespace Lattice
