#include "EditorViewportContainer.hpp"
#include "../state/AppState.hpp"
#include "CameraState.hpp"
#include "EditorScene.hpp"
#include "EditorViewport.hpp"
#include "KeyObserver.hpp"
#include <QSplitter>
#include <QVBoxLayout>

namespace Lattice {
namespace Editor {

EditorViewportContainer::EditorViewportContainer(const SP<State::AppState> &appState, QWidget *parent) : Viewport::ViewportContainer(parent),
                                                                                                         _appState(appState),
                                                                                                         _keyObserver(makeShared<KeyObserver>()) {
    setFocusPolicy(Qt::ClickFocus);

    setSplitMode(appState->viewportSplitMode());
    connect(appState.get(), &State::AppState::viewportSplitModeChanged, this, &EditorViewportContainer::setSplitMode);

    connect(this, &ViewportContainer::initialized, this, [this] {
        auto scene = makeShared<EditorScene>(_appState);
        _scene = scene;
        setRenderable(scene);
    });
}

void EditorViewportContainer::keyPressEvent(QKeyEvent *event) {
    if (_scene) {
        (*_scene)->keyPressEvent(event);
    }
}

void EditorViewportContainer::keyReleaseEvent(QKeyEvent *event) {
    if (_scene) {
        (*_scene)->keyReleaseEvent(event);
    }
}

void EditorViewportContainer::setSplitMode(State::ViewportSplitMode split) {
    auto oldLayout = layout();
    if (oldLayout) {
        for (int i = 0; i < oldLayout->count(); ++i) {
            auto item = oldLayout->itemAt(i);
            delete item->widget();
        }
        delete layout();
    }

    auto layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    std::vector<EditorViewport *> viewports;

    switch (split) {
    case State::ViewportSplitMode::Single: {
        auto viewport = new EditorViewport(_appState, _keyObserver);
        viewports = {viewport};
        layout->addWidget(viewport);
        break;
    }
    case State::ViewportSplitMode::LeftRight: {
        viewports = {
            new EditorViewport(_appState, _keyObserver), new EditorViewport(_appState, _keyObserver)};
        auto splitter = new QSplitter();
        for (auto &&v : viewports) {
            splitter->addWidget(v);
        }

        layout->addWidget(splitter);
        break;
    }
    case State::ViewportSplitMode::TopBottom: {
        viewports = {
            new EditorViewport(_appState, _keyObserver), new EditorViewport(_appState, _keyObserver)};
        auto splitter = new QSplitter();
        splitter->setOrientation(Qt::Vertical);
        for (auto &&v : viewports) {
            splitter->addWidget(v);
        }

        layout->addWidget(splitter);
        break;
    }
    case State::ViewportSplitMode::Four: {
        for (int i = 0; i < 4; ++i) {
            viewports.push_back(new EditorViewport(_appState, _keyObserver));
        }
        {
            viewports[0]->cameraState()->setProjection(Camera::Projection::Orthographic);
            viewports[0]->cameraState()->setPosition(glm::dvec3(0));
            viewports[0]->cameraState()->setOrientation(CameraState::Orientation::Top);
        }
        {
            viewports[2]->cameraState()->setProjection(Camera::Projection::Orthographic);
            viewports[2]->cameraState()->setPosition(glm::dvec3(0));
            viewports[2]->cameraState()->setOrientation(CameraState::Orientation::Front);
        }
        {
            viewports[3]->cameraState()->setProjection(Camera::Projection::Orthographic);
            viewports[3]->cameraState()->setPosition(glm::dvec3(0));
            viewports[3]->cameraState()->setOrientation(CameraState::Orientation::Right);
        }

        auto splitter = new QSplitter();
        splitter->setOrientation(Qt::Vertical);

        auto splitter0 = new QSplitter();
        splitter0->addWidget(viewports[0]);
        splitter0->addWidget(viewports[1]);
        auto splitter1 = new QSplitter();
        splitter1->addWidget(viewports[2]);
        splitter1->addWidget(viewports[3]);
        connect(splitter0, &QSplitter::splitterMoved, [splitter0, splitter1] {
            splitter1->setSizes(splitter0->sizes());
        });
        connect(splitter1, &QSplitter::splitterMoved, [splitter0, splitter1] {
            splitter0->setSizes(splitter1->sizes());
        });

        splitter->addWidget(splitter0);
        splitter->addWidget(splitter1);

        layout->addWidget(splitter);
        break;
    }
    }

    setLayout(layout);
    setViewports(std::vector<Viewport::Viewport *>(viewports.begin(), viewports.end()));

    for (auto &v : viewports) {
        v->setRenderable(_renderable);
    }

    update();
}

void EditorViewportContainer::setRenderable(const Opt<SP<Viewport::Renderable>> &renderable) {
    _renderable = renderable;
    for (auto &v : viewports()) {
        v->setRenderable(renderable);
    }
}

} // namespace Editor
} // namespace Lattice
