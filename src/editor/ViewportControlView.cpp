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
            connect(action, &QAction::triggered, camera.get(), [camera, projection = projection] (bool checked) {
                if (checked) {
                    camera->setProjection(projection);
                }
            });
            connect(camera.get(), &Camera::projectionChanged, action, [action, projection = projection](auto newProjection) {
                action->setChecked(projection == newProjection);
            });
            actionGroup->addAction(action);
        }
    }

    menu->addSeparator();

    {
        std::vector<std::pair<QString, Camera::Orientation>> orientations = {
            {tr("Front"), Camera::Orientation::Front},
            {tr("Back"), Camera::Orientation::Back},
            {tr("Right"), Camera::Orientation::Right},
            {tr("Left"), Camera::Orientation::Left},
            {tr("Top"), Camera::Orientation::Top},
            {tr("Bottom"), Camera::Orientation::Bottom},
        };

        for (auto&& [text, orientation] : orientations) {
            menu->addAction(text, this, [this, orientation = orientation] {
                _camera->setProjection(Camera::Projection::Orthographic);
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
            if (camera->projection() == Camera::Projection::Orthographic) {
                if (camera->isLookingOrientation(Camera::Orientation::Front)) {
                    return tr("Front");
                }
                if (camera->isLookingOrientation(Camera::Orientation::Back)) {
                    return tr("Back");
                }
                if (camera->isLookingOrientation(Camera::Orientation::Right)) {
                    return tr("Right");
                }
                if (camera->isLookingOrientation(Camera::Orientation::Left)) {
                    return tr("Left");
                }
                if (camera->isLookingOrientation(Camera::Orientation::Top)) {
                    return tr("Top");
                }
                if (camera->isLookingOrientation(Camera::Orientation::Bottom)) {
                    return tr("Bottom");
                }
                return tr("Orthographic");
            }
            return tr("Perspective");
        }();
        toolButton->setText(title);
    };

    connect(camera.get(), &Camera::changed, toolButton, updateMenuTitle);
    updateMenuTitle();

    auto layout = new QVBoxLayout();
    layout->addWidget(toolButton);
    setLayout(layout);
}

} // namespace Editor
} // namespace Lattice
