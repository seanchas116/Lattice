#include "PropertyView.hpp"
#include "ObjectPropertyView.hpp"
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

    auto itemPropertyView = new ObjectPropertyView(appState);
    layout->addWidget(itemPropertyView);
    auto meshPropertyView = new MeshPropertyView(appState);
    layout->addWidget(meshPropertyView);

    auto document = appState->document();

    auto update = [document, itemPropertyView, meshPropertyView] {
        meshPropertyView->setItem(document->editedObject());
        meshPropertyView->setVisible(!!document->editedObject() && !document->meshSelection().vertices.empty());
        itemPropertyView->setObjects(document->selectedObjects());
        itemPropertyView->setVisible(!document->selectedObjects().empty() && !document->editedObject());
    };
    connect(document.get(), &Document::Document::selectedObjectsChanged, this, update);
    connect(document.get(), &Document::Document::editedObjectChanged, this, update);
    connect(document.get(), &Document::Document::meshSelectionChanged, this, update);
    update();

    setLayout(layout);
}

} // namespace UI
} // namespace Lattice
