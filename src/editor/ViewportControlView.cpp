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
        std::vector<std::pair<QString, glm::dvec3>> orientations = {
            {tr("Front"), OrientationAngles::front},
            {tr("Back"), OrientationAngles::back},
            {tr("Right"), OrientationAngles::right},
            {tr("Left"), OrientationAngles::left},
            {tr("Top"), OrientationAngles::top},
            {tr("Bottom"), OrientationAngles::bottom},
        };

        for (auto&& [text, eulerAngles] : orientations) {
            menu->addAction(text, this, [this, eulerAngles = eulerAngles] {
                Location location;
                location.rotation = glm::dquat(eulerAngles);
                _camera->setProjection(Camera::Projection::Orthographic);
                _camera->setLocation(location);
            });
        }

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
