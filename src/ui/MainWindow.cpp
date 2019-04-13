#include "MainWindow.hpp"
#include "ObjectListView.hpp"
#include "PropertyView.hpp"
#include "../state/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../editor/EditorViewportContainer.hpp"
#include <QAction>
#include <QMenuBar>
#include <QUndoStack>
#include <QDockWidget>
#include <QToolBar>
#include <QToolButton>
#include <QKeyEvent>

namespace Lattice {
namespace UI {

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), _appState(makeShared<State::AppState>()) {
    resize(1000, 1000);
    auto editorWidget = new Editor::EditorViewportContainer(_appState);
    setCentralWidget(editorWidget);
    setupMenu();
    setupPanes();
    setupToolBar();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        _appState->document()->deleteSelectedObjects();
        break;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::setupToolBar() {
    auto appState = _appState.get();

    auto toolBar = new QToolBar(tr("Tools"));
    addToolBar(Qt::TopToolBarArea, toolBar);

    auto addMenu = new QMenu();
    addMenu->addAction(tr("Plane"), _appState.get(), &State::AppState::addPlane);
    addMenu->addAction(tr("Cube"), _appState.get(), &State::AppState::addCube);
    addMenu->addAction(tr("Circle"), _appState.get(), &State::AppState::addCircle);
    addMenu->addAction(tr("Sphere"), _appState.get(), &State::AppState::addSphere);
    addMenu->addAction(tr("Cone"), _appState.get(), &State::AppState::addCone);
    addMenu->addAction(tr("Cylinder"), _appState.get(), &State::AppState::addCylinder);
    addMenu->addSeparator();
    addMenu->addAction(tr("Text"), _appState.get(), &State::AppState::addText);
    addMenu->addAction(tr("Image Plane..."), _appState.get(), &State::AppState::addImagePlane);
    addMenu->addSeparator();
    addMenu->addAction(tr("Import..."), _appState.get(), &State::AppState::import);

    auto addMenuToolButton = new QToolButton();
    addMenuToolButton->setText(tr("Add"));
    addMenuToolButton->setMenu(addMenu);
    addMenuToolButton->setPopupMode(QToolButton::InstantPopup);
    toolBar->addWidget(addMenuToolButton);

    std::vector<std::pair<State::Tool, QString>> tools = {
        {State::Tool::Draw, tr("Draw")},
        {State::Tool::Extrude, tr("Extrude")},
        {State::Tool::InsetFaces, tr("Inset Faces")},
        {State::Tool::LoopCut, tr("Loop Cut")},
        {State::Tool::BorderSelect, tr("Border Select")},
    };

    for (auto [tool, text] : tools) {
        auto action = toolBar->addAction(text);
        action->setCheckable(true);
        connect(action, &QAction::toggled, appState, [appState, tool = tool] (bool checked) {
            if (checked) {
                appState->setTool(tool);
            } else if (appState->tool() == tool) {
                appState->setTool(State::Tool::None);
            }
        });
        connect(appState, &State::AppState::toolChanged, action, [action, tool = tool] (State::Tool newTool) {
            action->setChecked(newTool == tool);
        });
        action->setVisible(appState->document()->isEditing());
        connect(appState->document().get(), &Document::Document::isEditingChanged, action, [action] (bool isEditing) {
            action->setVisible(isEditing);
        });
    }

    auto spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer);

    auto isEditingAction = toolBar->addAction(tr("Edit"));
    isEditingAction->setCheckable(true);
    connect(_appState->document().get(), &Document::Document::isEditingChanged, isEditingAction, &QAction::setChecked);
    connect(isEditingAction, &QAction::toggled, _appState->document().get(), &Document::Document::setIsEditing);
    toolBar->addSeparator();

    // toggle manipulator handles
    auto isTranslateHandleVisible = toolBar->addAction(tr("Translate"));
    isTranslateHandleVisible->setCheckable(true);
    isTranslateHandleVisible->setChecked(_appState->isTranslateHandleVisible());
    connect(_appState.get(), &State::AppState::isTranslateHandleVisibleChanged, isTranslateHandleVisible, &QAction::setChecked);
    connect(isTranslateHandleVisible, &QAction::toggled, _appState.get(), &State::AppState::setTranslateHandleVisible);

    auto isRotateHandleVisible = toolBar->addAction(tr("Rotate"));
    isRotateHandleVisible->setCheckable(true);
    isRotateHandleVisible->setChecked(_appState->isRotateHandleVisible());
    connect(_appState.get(), &State::AppState::isRotateHandleVisibleChanged, isRotateHandleVisible, &QAction::setChecked);
    connect(isRotateHandleVisible, &QAction::toggled, _appState.get(), &State::AppState::setRotateHandleVisible);

    auto isScaleHandleVisibleAction = toolBar->addAction(tr("Scale"));
    isScaleHandleVisibleAction->setCheckable(true);
    isScaleHandleVisibleAction->setChecked(_appState->isScaleHandleVisible());
    connect(_appState.get(), &State::AppState::isScaleHandleVisibleChanged, isScaleHandleVisibleAction, &QAction::setChecked);
    connect(isScaleHandleVisibleAction, &QAction::toggled, _appState.get(), &State::AppState::setScaleHandleVisible);

    toolBar->addSeparator();

    // toggle selectabilities

    auto isVertexVisibleAction = toolBar->addAction(tr("Vertex"));
    isVertexVisibleAction->setCheckable(true);
    isVertexVisibleAction->setChecked(_appState->isVertexSelectable());
    connect(_appState.get(), &State::AppState::isVertexSelectableChanged, isVertexVisibleAction, &QAction::setChecked);
    connect(isVertexVisibleAction, &QAction::toggled, _appState.get(), &State::AppState::setVertexSelectable);

    auto isEdgeVisibleAction = toolBar->addAction(tr("Edge"));
    isEdgeVisibleAction->setCheckable(true);
    isEdgeVisibleAction->setChecked(_appState->isEdgeSelectable());
    connect(_appState.get(), &State::AppState::isEdgeSelectableChanged, isEdgeVisibleAction, &QAction::setChecked);
    connect(isEdgeVisibleAction, &QAction::toggled, _appState.get(), &State::AppState::setEdgeSelectable);

    auto isFaceVisibleAction = toolBar->addAction(tr("Face"));
    isFaceVisibleAction->setCheckable(true);
    isFaceVisibleAction->setChecked(_appState->isFaceSelectable());
    connect(_appState.get(), &State::AppState::isFaceSelectableChanged, isFaceVisibleAction, &QAction::setChecked);
    connect(isFaceVisibleAction, &QAction::toggled, _appState.get(), &State::AppState::setFaceSelectable);
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
        connect(importAction, &QAction::triggered, _appState.get(), &State::AppState::import);

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
        connect(selectAllAction, &QAction::triggered, _appState.get(), &State::AppState::selectAll);
        selectAllAction->setShortcut(QKeySequence::SelectAll);
        editMenu->addAction(selectAllAction);

        auto deselectAllAction = new QAction(tr("Deselect All"), this);
        deselectAllAction->setShortcut(QKeySequence("Ctrl+D"));
        connect(deselectAllAction, &QAction::triggered, _appState.get(), &State::AppState::deselectAll);
        editMenu->addAction(deselectAllAction);
    }

    {
        auto windowMenu = menuBar->addMenu(tr("&Window"));

        {
            auto viewportsMenu = windowMenu->addMenu(tr("Viewports"));
            std::vector<std::pair<State::ViewportSplitMode, QString>> entries = {
                {State::ViewportSplitMode::Single, tr("Single")},
                {State::ViewportSplitMode::LeftRight, tr("Left / Right")},
                {State::ViewportSplitMode::TopBottom, tr("Top / Bottom")},
                {State::ViewportSplitMode::Four, tr("Four")},
            };
            std::unordered_map<State::ViewportSplitMode, QAction*> actions;

            auto group = new QActionGroup(this);
            group->setExclusive(true);

            for (auto [mode, text] : entries) {
                auto action = viewportsMenu->addAction(text);
                group->addAction(action);
                connect(action, &QAction::triggered, this, [this, mode = mode] { // ← clang bug?
                    _appState->setViewportSplit(mode);
                });
                actions[mode] = action;
            }

            auto onChange = [actions](State::ViewportSplitMode mode) {
                actions.at(mode)->setChecked(true);
            };
            onChange(_appState->viewportSplitMode());
            connect(_appState.get(), &State::AppState::viewportSplitModeChanged, this, onChange);
        }

        windowMenu->addSeparator();

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
        dockWidget->setWindowTitle(tr("Objects"));
        dockWidget->setWidget(new ObjectListView(_appState));
        addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    }

    {
        auto dockWidget = new QDockWidget();
        dockWidget->setWindowTitle(tr("Property"));

        dockWidget->setWidget(new PropertyView(_appState));
        addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
    }
}

}
} // namespace Lattice
