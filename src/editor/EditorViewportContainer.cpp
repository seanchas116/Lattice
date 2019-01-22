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

    std::vector<EditorViewport*> viewports = {
        new EditorViewport(appState), new EditorViewport(appState)
    };
    auto layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    for (auto&& v : viewports) {
        layout->addWidget(v);
    }
    setLayout(layout);

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
