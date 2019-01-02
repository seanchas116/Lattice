#include "MainWindow.hpp"
#include "AppState.hpp"
#include "ItemListPane.hpp"
#include "ItemPropertyPane.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../viewport/ViewportWidget.hpp"
#include <QAction>
#include <QMenuBar>
#include <QUndoStack>
#include <QDockWidget>
#include <QToolBar>
#include <QToolButton>

namespace Lattice {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    _appState = std::make_shared<AppState>();
    auto viewportWidget = new ViewportWidget(_appState);
    setCentralWidget(viewportWidget);
    setupMenu();
    setupPanes();
    setupToolBar();
}

void MainWindow::setupToolBar() {
    auto toolBar = addToolBar(tr("Tools"));

    auto addMenu = new QMenu();
    addMenu->addAction(tr("Plane"), _appState.get(), &AppState::addPlane);
    addMenu->addAction(tr("Cube"), _appState.get(), &AppState::addCube);
    addMenu->addAction(tr("Circle"), _appState.get(), &AppState::addCircle);
    addMenu->addAction(tr("Sphere"), _appState.get(), &AppState::addSphere);
    addMenu->addAction(tr("Cylinder"), _appState.get(), &AppState::addCylinder);
    addMenu->addSeparator();
    addMenu->addAction(tr("Text"), _appState.get(), &AppState::addText);
    addMenu->addAction(tr("Image Plane..."), _appState.get(), &AppState::addImagePlane);
    addMenu->addSeparator();
    addMenu->addAction(tr("Import..."), _appState.get(), &AppState::import);

    auto addMenuToolButton = new QToolButton();
    addMenuToolButton->setText(tr("Add"));
    addMenuToolButton->setMenu(addMenu);
    addMenuToolButton->setPopupMode(QToolButton::InstantPopup);
    toolBar->addWidget(addMenuToolButton);

    auto modifyMenu = new QMenu();
    modifyMenu->addAction(tr("Extrude"));
    modifyMenu->addAction(tr("Inset Faces"));
    modifyMenu->addAction(tr("Loop Cut"));

    auto modifyMenuToolButton = new QToolButton();
    modifyMenuToolButton->setText(tr("Modify"));
    modifyMenuToolButton->setMenu(modifyMenu);
    modifyMenuToolButton->setPopupMode(QToolButton::InstantPopup);
    toolBar->addWidget(modifyMenuToolButton);

    auto spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer);

    // toggle view visibility

    {
        auto isVertexVisibleAction = new QAction(tr("Vertex"));
        isVertexVisibleAction->setCheckable(true);
        isVertexVisibleAction->setChecked(_appState->isVertexVisible());
        connect(_appState.get(), &AppState::isVertexVisibleChanged, isVertexVisibleAction, &QAction::setChecked);
        connect(isVertexVisibleAction, &QAction::toggled, _appState.get(), &AppState::setIsVertexVisible);
        toolBar->addAction(isVertexVisibleAction);
    }

    {
        auto isEdgeVisibleAction = new QAction(tr("Edge"));
        isEdgeVisibleAction->setCheckable(true);
        isEdgeVisibleAction->setChecked(_appState->isEdgeVisible());
        connect(_appState.get(), &AppState::isEdgeVisibleChanged, isEdgeVisibleAction, &QAction::setChecked);
        connect(isEdgeVisibleAction, &QAction::toggled, _appState.get(), &AppState::setIsEdgeVisible);
        toolBar->addAction(isEdgeVisibleAction);
    }

    {
        auto isFaceVisibleAction = new QAction(tr("Face"));
        isFaceVisibleAction->setCheckable(true);
        isFaceVisibleAction->setChecked(_appState->isFaceVisible());
        connect(_appState.get(), &AppState::isFaceVisibleChanged, isFaceVisibleAction, &QAction::setChecked);
        connect(isFaceVisibleAction, &QAction::toggled, _appState.get(), &AppState::setIsFaceVisible);
        toolBar->addAction(isFaceVisibleAction);
    }
}

void MainWindow::setupMenu() {
    auto menuBar = new QMenuBar();

    {
        auto fileMenu = menuBar->addMenu(tr("&File"));

        auto newAction = new QAction(tr("New..."), this);
        newAction->setShortcut(QKeySequence::New);
        fileMenu->addAction(newAction);

        auto openAction = new QAction(tr("Open..."), this);
        openAction->setShortcut(QKeySequence::Open);
        fileMenu->addAction(openAction);

        fileMenu->addSeparator();

        auto closeAction = new QAction(tr("Close"), this);
        closeAction->setShortcut(QKeySequence::Close);
        fileMenu->addAction(closeAction);

        fileMenu->addSeparator();

        auto saveAction = new QAction(tr("Save"), this);
        saveAction->setShortcut(QKeySequence::Save);
        fileMenu->addAction(saveAction);

        auto saveAsAction = new QAction(tr("Save As..."), this);
        saveAsAction->setShortcut(QKeySequence::SaveAs);
        fileMenu->addAction(saveAsAction);

        fileMenu->addSeparator();

        auto importAction = new QAction(tr("Import..."), this);
        fileMenu->addAction(importAction);
        connect(importAction, &QAction::triggered, _appState.get(), &AppState::import);

        auto exportAction = new QAction(tr("Export..."), this);
        fileMenu->addAction(exportAction);
    }

    {
        auto editMenu = menuBar->addMenu(tr("&Edit"));

        auto undoStack = _appState->document()->history()->undoStack();

        auto undoAction = undoStack->createUndoAction(this);
        undoAction->setShortcut(QKeySequence::Undo);
        editMenu->addAction(undoAction);

        auto redoAction = undoStack->createRedoAction(this);
        redoAction->setShortcut(QKeySequence::Redo);
        editMenu->addAction(redoAction);

        editMenu->addSeparator();

        auto cutAction = new QAction(tr("Cut"), this);
        cutAction->setShortcut(QKeySequence::Cut);
        editMenu->addAction(cutAction);

        auto copyAction = new QAction(tr("Copy"), this);
        copyAction->setShortcut(QKeySequence::Copy);
        editMenu->addAction(copyAction);

        auto pasteAction = new QAction(tr("Paste"), this);
        pasteAction->setShortcut(QKeySequence::Paste);
        editMenu->addAction(pasteAction);

        editMenu->addSeparator();

        auto selectAllAction = new QAction(tr("Select All"), this);
        selectAllAction->setShortcut(QKeySequence::SelectAll);
        editMenu->addAction(selectAllAction);

        auto deselectAllAction = new QAction(tr("Deselect All"), this);
        deselectAllAction->setShortcut(QKeySequence("Ctrl+D"));
        editMenu->addAction(deselectAllAction);
    }

    {
        auto windowMenu = menuBar->addMenu(tr("&Window"));
        windowMenu->addAction(tr("Minimize"), this, &QWidget::showMinimized, QKeySequence("Ctrl+M"));
        windowMenu->addAction(tr("Zoom"), this, &QWidget::showMaximized);
    }

    {
        //TODO: add default help menu (in Mac)
    }

    setMenuBar(menuBar);
}

void MainWindow::setupPanes() {
    {
        auto dockWidget = new QDockWidget();
        dockWidget->setWindowTitle(tr("Items"));
        dockWidget->setWidget(new ItemListPane(_appState));
        addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    }

    {
        auto dockWidget = new QDockWidget();
        dockWidget->setWindowTitle(tr("Property"));
        dockWidget->setWidget(new ItemPropertyPane(_appState));
        addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    }
}

} // namespace Lattice
