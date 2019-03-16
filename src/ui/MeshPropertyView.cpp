#include "MeshPropertyView.hpp"
#include "../document/MeshItem.hpp"
#include "../document/Document.hpp"
#include "../state/AppState.hpp"

namespace Lattice {
namespace UI {

MeshPropertyView::MeshPropertyView(const SP<State::AppState> &appState, QWidget *parent) :
    QWidget(parent),
    _appState(appState)
{
    setItem(appState->document()->editedItem());
    connect(appState->document().get(), &Document::Document::editedItemChanged, this, &MeshPropertyView::setItem);
}

void MeshPropertyView::setItem(const Opt<SP<Document::MeshItem>> &maybeItem) {
    disconnect(_connection);
    _item = maybeItem;
    if (!maybeItem) {
        return;
    }
    auto item = *maybeItem;

    _connection = connect(item->mesh().get(), &Mesh::Mesh::changed, this, [this] {
        updateViews();
    });
    updateViews();
}

void MeshPropertyView::updateViews() {
}

} // namespace UI
} // namespace Lattice
