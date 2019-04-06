#include "EditorViewportContainer.hpp"
#include "EditorViewport.hpp"
#include "EditorScene.hpp"
#include "KeyObserver.hpp"
#include "../state/AppState.hpp"
#include <QVBoxLayout>
#include <QSplitter>

namespace Lattice {
namespace Editor {

EditorViewportContainer::EditorViewportContainer(const SP<State::AppState> &appState, QWidget *parent) :
    Viewport::ViewportContainer(parent),
    _appState(appState),
    _keyObserver(makeShared<KeyObserver>())
{
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

    std::vector<Viewport::Viewport*> viewports;

    switch (split) {
    case State::ViewportSplitMode::Single: {
        auto viewport = new EditorViewport(_appState, _keyObserver);
        viewports = {viewport};
        layout->addWidget(viewport);
        break;
    }
    case State::ViewportSplitMode::LeftRight: {
        viewports = {
            new EditorViewport(_appState, _keyObserver), new EditorViewport(_appState, _keyObserver)
        };
        auto splitter = new QSplitter();
        for (auto&& v : viewports) {
            splitter->addWidget(v);
        }

        layout->addWidget(splitter);
        break;
    }
    case State::ViewportSplitMode::TopBottom: {
        viewports = {
            new EditorViewport(_appState, _keyObserver), new EditorViewport(_appState, _keyObserver)
        };
        auto splitter = new QSplitter();
        splitter->setOrientation(Qt::Vertical);
        for (auto&& v : viewports) {
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
            Location location;
            location.rotation = glm::dquat(OrientationAngles::top);
            viewports[0]->camera()->setProjection(Camera::Projection::Orthographic);
            viewports[0]->camera()->setLocation(location);
        }
        {
            Location location;
            location.rotation = glm::dquat(OrientationAngles::front);
            viewports[2]->camera()->setProjection(Camera::Projection::Orthographic);
            viewports[2]->camera()->setLocation(location);
        }
        {
            Location location;
            location.rotation = glm::dquat(OrientationAngles::right);
            viewports[3]->camera()->setProjection(Camera::Projection::Orthographic);
            viewports[3]->camera()->setLocation(location);
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
    setViewports(viewports);

    for (auto& v : viewports) {
        v->setRenderable(_renderable);
    }

    update();
}

void EditorViewportContainer::setRenderable(const Opt<SP<Viewport::RenderableObject> > &renderable) {
    _renderable = renderable;
    for (auto& v : viewports()) {
        v->setRenderable(renderable);
    }
}

}
} // namespace Lattice
