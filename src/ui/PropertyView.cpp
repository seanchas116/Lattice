#include "PropertyView.hpp"
#include "ItemPropertyView.hpp"
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

    _itemPropertyView = new ItemPropertyView(appState);
    connect(appState->document().get(), &Document::Document::selectedItemsChanged, this, &PropertyView::handleSelectedItemsChanged);
    handleSelectedItemsChanged(appState->document()->selectedItems());

    layout->addWidget(_itemPropertyView);

    setLayout(layout);
}

void PropertyView::handleSelectedItemsChanged(const std::unordered_set<SP<Document::Item>> &items) {
    _itemPropertyView->setItems(items);
    _itemPropertyView->setVisible(!items.empty());
}


} // namespace UI
} // namespace Lattice
