#include "EditorViewportContainer.hpp"
#include "EditorViewport.hpp"
#include "EditorScene.hpp"
#include "KeyObserver.hpp"
#include <QVBoxLayout>
#include <QSplitter>

namespace Lattice::Editor {

EditorViewportContainer::EditorViewportContainer(const SP<UI::AppState> &appState, QWidget *parent) :
    Render::ViewportContainer(parent),
    _appState(appState),
    _keyObserver(makeShared<KeyObserver>())
{
    setFocusPolicy(Qt::ClickFocus);

    std::vector<Render::Viewport*> viewports = {
        new EditorViewport(appState, _keyObserver), new EditorViewport(appState, _keyObserver)
    };
    auto splitter = new QSplitter();

    for (auto&& v : viewports) {
        splitter->addWidget(v);
    }

    auto layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(splitter);

    setLayout(layout);

    setViewports(viewports);

    connect(this, &ViewportContainer::initialized, this, [this, viewports] {
        auto scene = makeShared<EditorScene>(_appState);
        _scene = scene;
        connect(scene.get(), &EditorScene::updateRequested, this, [this] { update(); });
        for (auto&& v : viewports) {
            connect(this, &ViewportContainer::aboutToBePainted, v, [scene, v] {
                v->setRenderables(scene->updateRenderables());
            });
        }
    });
}

} // namespace Lattice
