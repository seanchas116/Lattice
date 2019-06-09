#include "ViewportControlView.hpp"
#include <QRadioButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QActionGroup>

namespace Lattice {
namespace Editor {

ViewportControlView::ViewportControlView(const SP<OldCamera> &camera, QWidget *parent) : QWidget(parent), _camera(camera) {
    auto menu = new QMenu(this);

    {
        auto actionGroup = new QActionGroup(this);
        actionGroup->setExclusive(true);

        std::vector<std::pair<OldCamera::Projection, QString>> projections = {
            {OldCamera::Projection::Perspective, tr("Perspective")},
            {OldCamera::Projection::Orthographic, tr("Orthographic")},
        };
        for (auto&& [projection, text] : projections) {
            auto action= menu->addAction(text);
            action->setCheckable(true);
            action->setChecked(projection == _camera->projection());
            connect(action, &QAction::triggered, camera.get(), [camera, projection = projection] (bool checked) {
                if (checked) {
                    camera->setProjection(projection);
                }
            });
            connect(camera.get(), &OldCamera::projectionChanged, action, [action, projection = projection](auto newProjection) {
                action->setChecked(projection == newProjection);
            });
            actionGroup->addAction(action);
        }
    }

    menu->addSeparator();

    {
        std::vector<std::pair<QString, OldCamera::Orientation>> orientations = {
            {tr("Front"), OldCamera::Orientation::Front},
            {tr("Back"), OldCamera::Orientation::Back},
            {tr("Right"), OldCamera::Orientation::Right},
            {tr("Left"), OldCamera::Orientation::Left},
            {tr("Top"), OldCamera::Orientation::Top},
            {tr("Bottom"), OldCamera::Orientation::Bottom},
        };

        for (auto&& [text, orientation] : orientations) {
            menu->addAction(text, this, [this, orientation = orientation] {
                _camera->setProjection(OldCamera::Projection::Orthographic);
                _camera->setLocation(Location());
                _camera->lookOrientation(orientation);
            });
        }

        menu->addSeparator();

        menu->addAction(tr("Reset"));
    }

    auto toolButton = new QToolButton();
    toolButton->setText(tr("Menu"));
    toolButton->setMenu(menu);
    toolButton->setPopupMode(QToolButton::InstantPopup);

    auto updateMenuTitle = [camera, toolButton] {
        auto title = [&] {
            if (camera->projection() == OldCamera::Projection::Orthographic) {
                if (camera->isLookingOrientation(OldCamera::Orientation::Front)) {
                    return tr("Front");
                }
                if (camera->isLookingOrientation(OldCamera::Orientation::Back)) {
                    return tr("Back");
                }
                if (camera->isLookingOrientation(OldCamera::Orientation::Right)) {
                    return tr("Right");
                }
                if (camera->isLookingOrientation(OldCamera::Orientation::Left)) {
                    return tr("Left");
                }
                if (camera->isLookingOrientation(OldCamera::Orientation::Top)) {
                    return tr("Top");
                }
                if (camera->isLookingOrientation(OldCamera::Orientation::Bottom)) {
                    return tr("Bottom");
                }
                return tr("Orthographic");
            }
            return tr("Perspective");
        }();
        toolButton->setText(title);
    };

    connect(camera.get(), &OldCamera::changed, toolButton, updateMenuTitle);
    updateMenuTitle();

    auto layout = new QVBoxLayout();
    layout->addWidget(toolButton);
    setLayout(layout);
}

} // namespace Editor
} // namespace Lattice
