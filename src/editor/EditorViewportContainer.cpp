#include "EditorViewportContainer.hpp"
#include "EditorViewport.hpp"
#include "EditorScene.hpp"
#include <QVBoxLayout>

namespace Lattice::Editor {

EditorViewportContainer::EditorViewportContainer(const SP<UI::AppState> &appState, QWidget *parent) :
    Render::ViewportContainer(parent),
    _appState(appState)
{
    setFocusPolicy(Qt::ClickFocus);

    auto viewport = new EditorViewport(appState);
    auto layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->addWidget(viewport);
    setLayout(layout);

    connect(this, &ViewportContainer::initialized, this, [this, viewport] {
        auto scene = makeShared<EditorScene>(_appState);
        _scene = scene;
        connect(scene.get(), &EditorScene::updateRequested, this, [this] { update(); });
        connect(this, &ViewportContainer::aboutToBePainted, viewport, [scene, viewport] {
            viewport->setRenderables(scene->updateRenderables());
        });
    });
}

} // namespace Lattice
