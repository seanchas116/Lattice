#include "ItemListView.hpp"
#include "ItemModel.hpp"
#include "ItemSelectionModel.hpp"
#include "../state/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include <QTreeView>
#include <QVBoxLayout>
#include <QMenu>

namespace Lattice::UI {

ItemListView::ItemListView(const SP<State::AppState> &appState, QWidget *parent) :
    QWidget(parent),
    _appState(appState)
{
    auto treeView = new QTreeView();
    treeView->setHeaderHidden(true);
    treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeView->setDragDropMode(QAbstractItemView::DragDrop);
    treeView->setDefaultDropAction(Qt::MoveAction);
    treeView->setDropIndicatorShown(true);
    treeView->setAcceptDrops(true);

    auto model = new ItemModel(appState->document(), this);
    auto selectionModel = new ItemSelectionModel(model, this);
    treeView->setModel(model);
    treeView->setSelectionModel(selectionModel);

    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, &QWidget::customContextMenuRequested, this, [=](const QPoint& p) {
        auto index = treeView->indexAt(p);
        if (index.isValid()) {
            QMenu contextMenu;
            contextMenu.addAction(tr("Delete"), _appState.get(), &State::AppState::deleteItems);
            contextMenu.exec(treeView->mapToGlobal(p));
        }
    });

    auto layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->addWidget(treeView);
    setLayout(layout);
}

} // namespace Shapecraft
