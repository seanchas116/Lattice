#include "EditorViewportContainer.hpp"
#include "EditorViewport.hpp"
#include "EditorScene.hpp"
#include "KeyObserver.hpp"
#include "../ui/AppState.hpp"
#include <QVBoxLayout>
#include <QSplitter>

namespace Lattice::Editor {

EditorViewportContainer::EditorViewportContainer(const SP<UI::AppState> &appState, QWidget *parent) :
    Render::ViewportContainer(parent),
    _appState(appState),
    _keyObserver(makeShared<KeyObserver>())
{
    setFocusPolicy(Qt::ClickFocus);

    setSplitMode(appState->viewportSplit());
    connect(appState.get(), &UI::AppState::viewportSplitChanged, this, &EditorViewportContainer::setSplitMode);

    connect(this, &ViewportContainer::initialized, this, [this] {
        auto scene = makeShared<EditorScene>(_appState);
        _scene = scene;
        connect(scene.get(), &EditorScene::updateRequested, this, [this] { update(); });
        connect(this, &ViewportContainer::aboutToBePainted, this, [this, scene] {
            setRenderables(scene->updateRenderables());
        });
    });
}

void EditorViewportContainer::setSplitMode(UI::ViewportSplit split) {
    qDeleteAll(children());
    delete layout();

    auto layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    std::vector<Render::Viewport*> viewports;

    switch (split) {
    case UI::ViewportSplit::Single: {
        auto viewport = new EditorViewport(_appState, _keyObserver);
        viewports = {viewport};
        layout->addWidget(viewport);
        break;
    }
    case UI::ViewportSplit::LeftRight: {
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
    case UI::ViewportSplit::TopBottom: {
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
    case UI::ViewportSplit::Four: {
        for (int i = 0; i < 4; ++i) {
            viewports.push_back(new EditorViewport(_appState, _keyObserver));
        }
        auto splitter = new QSplitter();
        splitter->setOrientation(Qt::Vertical);

        auto splitter0 = new QSplitter();
        splitter0->addWidget(viewports[0]);
        splitter0->addWidget(viewports[1]);
        auto splitter1 = new QSplitter();
        splitter1->addWidget(viewports[2]);
        splitter1->addWidget(viewports[3]);

        splitter->addWidget(splitter0);
        splitter->addWidget(splitter1);

        layout->addWidget(splitter);
        break;
    }
    }

    setLayout(layout);
    setViewports(viewports);

    for (auto& v : viewports) {
        v->setRenderables(_renderables);
    }

    update();
}

void EditorViewportContainer::setRenderables(const std::vector<SP<Render::Renderable> > &renderables) {
    _renderables = renderables;
    for (auto& v : viewports()) {
        v->setRenderables(renderables);
    }
}

} // namespace Lattice
