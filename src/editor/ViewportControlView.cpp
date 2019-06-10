#include "ViewportControlView.hpp"
#include "CameraState.hpp"
#include <QRadioButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QActionGroup>

namespace Lattice {
namespace Editor {

ViewportControlView::ViewportControlView(const SP<CameraState> &cameraState, QWidget *parent) : QWidget(parent), _cameraState(cameraState) {
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
            action->setChecked(projection == cameraState->projection());
            connect(action, &QAction::triggered, cameraState.get(), [cameraState, projection = projection] (bool checked) {
                if (checked) {
                    cameraState->setProjection(projection);
                }
            });
            connect(cameraState.get(), &CameraState::projectionChanged, action, [action, projection = projection](auto newProjection) {
                action->setChecked(projection == newProjection);
            });
            actionGroup->addAction(action);
        }
    }

    menu->addSeparator();

    {
        std::vector<std::pair<QString, CameraState::Orientation>> orientations = {
            {tr("Front"), CameraState::Orientation::Front},
            {tr("Back"), CameraState::Orientation::Back},
            {tr("Right"), CameraState::Orientation::Right},
            {tr("Left"), CameraState::Orientation::Left},
            {tr("Top"), CameraState::Orientation::Top},
            {tr("Bottom"), CameraState::Orientation::Bottom},
        };

        for (auto&& [text, orientation] : orientations) {
            menu->addAction(text, this, [this, orientation = orientation] {
                _cameraState->setProjection(Camera::Projection::Orthographic);
                _cameraState->setOrientation(orientation);
            });
        }

        menu->addSeparator();

        menu->addAction(tr("Reset"));
    }

    auto toolButton = new QToolButton();
    toolButton->setText(tr("Menu"));
    toolButton->setMenu(menu);
    toolButton->setPopupMode(QToolButton::InstantPopup);

    auto updateMenuTitle = [cameraState, toolButton] {
        auto title = [&] {
            if (cameraState->projection() == Camera::Projection::Orthographic) {
                if (cameraState->orientation() == CameraState::Orientation::Front) {
                    return tr("Front");
                }
                if (cameraState->orientation() == CameraState::Orientation::Back) {
                    return tr("Back");
                }
                if (cameraState->orientation() == CameraState::Orientation::Right) {
                    return tr("Right");
                }
                if (cameraState->orientation() == CameraState::Orientation::Left) {
                    return tr("Left");
                }
                if (cameraState->orientation() == CameraState::Orientation::Top) {
                    return tr("Top");
                }
                if (cameraState->orientation() == CameraState::Orientation::Bottom) {
                    return tr("Bottom");
                }
                return tr("Orthographic");
            }
            return tr("Perspective");
        }();
        toolButton->setText(title);
    };

    connect(cameraState.get(), &CameraState::orientationChanged, toolButton, updateMenuTitle);
    connect(cameraState.get(), &CameraState::projectionChanged, toolButton, updateMenuTitle);
    updateMenuTitle();

    auto layout = new QVBoxLayout();
    layout->addWidget(toolButton);
    setLayout(layout);
}

} // namespace Editor
} // namespace Lattice
