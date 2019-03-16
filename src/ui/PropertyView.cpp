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

    auto handleSelectedItemsChanged = [appState, itemPropertyView] {
        itemPropertyView->setItems(appState->document()->selectedItems());
        itemPropertyView->setVisible(!appState->document()->selectedItems().empty() && !appState->document()->editedItem());
    };
    auto handleEditedItemChanged = [appState, itemPropertyView, meshPropertyView] {
        meshPropertyView->setItem(appState->document()->editedItem());
        meshPropertyView->setVisible(!!appState->document()->editedItem());
        itemPropertyView->setVisible(!appState->document()->selectedItems().empty() && !appState->document()->editedItem());
    };
    connect(appState->document().get(), &Document::Document::selectedItemsChanged, this, handleSelectedItemsChanged);
    handleSelectedItemsChanged();

    connect(appState->document().get(), &Document::Document::editedItemChanged, this, handleEditedItemChanged);
    handleEditedItemChanged();

    setLayout(layout);
}

} // namespace UI
} // namespace Lattice
