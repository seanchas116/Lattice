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

    auto objectPropertyView = new ObjectPropertyView(appState);
    layout->addWidget(objectPropertyView);
    auto meshPropertyView = new MeshPropertyView(appState);
    layout->addWidget(meshPropertyView);

    auto document = appState->document();

    auto update = [appState, document, objectPropertyView, meshPropertyView] {
        meshPropertyView->setMeshEditState(appState->meshEditState());
        meshPropertyView->setVisible(!!appState->meshEditState() && !document->meshSelection().vertices.empty());
        objectPropertyView->setObjects(document->selectedObjects());
        objectPropertyView->setVisible(!document->selectedObjects().empty() && !appState->meshEditState());
    };
    connect(document.get(), &Document::Document::selectedObjectsChanged, this, update);
    connect(appState.get(), &State::AppState::meshEditStateChanged, this, update);
    connect(document.get(), &Document::Document::meshSelectionChanged, this, update);
    update();

    setLayout(layout);
}

} // namespace UI
} // namespace Lattice
