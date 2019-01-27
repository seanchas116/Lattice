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
            {tr("Front"), glm::vec3(0, 0, 0)},
            {tr("Back"), glm::vec3(0, M_PI, 0)},
            {tr("Right"), glm::vec3(0, M_PI * 0.5, 0)},
            {tr("Left"), glm::vec3(0, M_PI * 1.5, 0)},
            {tr("Top"), glm::vec3(M_PI * -0.5, 0, 0)},
            {tr("Bottom"), glm::vec3(M_PI * 0.5, 0, 0)},
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
