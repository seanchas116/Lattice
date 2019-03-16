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

    auto itemPropertyView = new ItemPropertyView();
    connect(appState->document().get(), &Document::Document::selectedItemsChanged, itemPropertyView, &ItemPropertyView::setItems);
    itemPropertyView->setItems(appState->document()->selectedItems());

    layout->addWidget(itemPropertyView);

    setLayout(layout);
}

} // namespace UI
} // namespace Lattice
