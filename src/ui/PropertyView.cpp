#include "PropertyView.hpp"
#include "ItemPropertyView.hpp"
#include "MeshPropertyView.hpp"
#include "../state/AppState.hpp"
#include "../document/Document.hpp"
#include <QVBoxLayout>

namespace Lattice {
namespace UI {

PropertyView::PropertyView(const SP<State::AppState> &appState, QWidget *parent) :
    QWidget(parent),
    _appState(appState)
{
    auto layout = new QVBoxLayout();
    layout->setMargin(0);

    auto itemPropertyView = new ItemPropertyView(appState);
    layout->addWidget(itemPropertyView);
    auto meshPropertyView = new MeshPropertyView(appState);
    layout->addWidget(meshPropertyView);

    auto document = appState->document();

    auto update = [document, itemPropertyView, meshPropertyView] {
        meshPropertyView->setItem(document->editedItem());
        meshPropertyView->setVisible(!!document->editedItem() && !document->meshSelection().vertices.empty());
        itemPropertyView->setItems(document->selectedItems());
        itemPropertyView->setVisible(!document->selectedItems().empty() && !document->editedItem());
    };
    connect(document.get(), &Document::Document::selectedItemsChanged, this, update);
    connect(document.get(), &Document::Document::editedItemChanged, this, update);
    connect(document.get(), &Document::Document::meshSelectionChanged, this, update);
    update();

    setLayout(layout);
}

} // namespace UI
} // namespace Lattice
