#include "ViewportControlView.hpp"
#include <QRadioButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QActionGroup>

namespace Lattice {
namespace Editor {

ViewportControlView::ViewportControlView(const SP<Camera> &camera, QWidget *parent) : QWidget(parent), _camera(camera) {
    auto menu = new QMenu(this);

    {
        auto actionGroup = new QActionGroup(this);
        actionGroup->setExclusive(true);

        std::vector<std::pair<Camera::Projection, QString>> projections = {
            {Camera::Projection::Perspective, tr("Perspective")},
            {Camera::Projection::Orthographic, tr("Orthographic")},
        };
        for (auto&& [projection, text] : projections) {
            auto action= menu->addAction(text);
            action->setCheckable(true);
            action->setChecked(projection == _camera->projection());
            connect(action, &QAction::triggered, this, [this, projection = projection] (bool checked) {
                if (checked) {
                    _camera->setProjection(projection);
                }
            });
            actionGroup->addAction(action);
        }
    }

    menu->addSeparator();

    {
        menu->addAction(tr("Front"));
        menu->addAction(tr("Back"));
        menu->addAction(tr("Right"));
        menu->addAction(tr("Left"));
        menu->addAction(tr("Top"));
        menu->addAction(tr("Bottom"));

        menu->addSeparator();

        menu->addAction(tr("Reset"));
    }


    auto toolButton = new QToolButton();
    toolButton->setText(tr("Menu"));
    toolButton->setMenu(menu);
    toolButton->setPopupMode(QToolButton::InstantPopup);

    auto layout = new QVBoxLayout();
    layout->addWidget(toolButton);
    setLayout(layout);
}

} // namespace Editor
} // namespace Lattice
