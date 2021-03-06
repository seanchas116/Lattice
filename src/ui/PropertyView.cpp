#include "PropertyView.hpp"
#include "../document/Document.hpp"
#include "../state/AppState.hpp"
#include "MeshPropertyView.hpp"
#include "ObjectPropertyView.hpp"
#include <QTabWidget>
#include <QVBoxLayout>

namespace Lattice {
namespace UI {

PropertyView::PropertyView(const SP<State::AppState> &appState, QWidget *parent) : QWidget(parent),
                                                                                   _appState(appState) {
    auto layout = new QVBoxLayout();
    layout->setMargin(0);

    auto tabWidget = new QTabWidget();

    auto objectPropertyView = new ObjectPropertyView(appState);
    tabWidget->addTab(objectPropertyView, tr("Object"));
    auto meshPropertyView = new MeshPropertyView(appState);
    tabWidget->addTab(meshPropertyView, tr("Mesh"));

    layout->addWidget(tabWidget);

    setLayout(layout);

    connect(appState.get(), &State::AppState::meshEditStateChanged, this, [tabWidget, meshPropertyView, objectPropertyView](auto state) {
        if (state) {
            tabWidget->setCurrentWidget(meshPropertyView);
        } else {
            tabWidget->setCurrentWidget(objectPropertyView);
        }
    });
}

} // namespace UI
} // namespace Lattice
