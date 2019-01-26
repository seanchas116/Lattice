#include "ViewportControlView.hpp"
#include <QRadioButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QActionGroup>

namespace Lattice {
namespace Editor {

ViewportControlView::ViewportControlView(QWidget *parent) : QWidget(parent) {
    auto menu = new QMenu(this);

    {
        auto actionPerspective = menu->addAction(tr("Perspective"));
        actionPerspective->setCheckable(true);
        actionPerspective->setChecked(true);

        auto actionOrthogonal= menu->addAction(tr("Orthogonal"));
        actionOrthogonal->setCheckable(true);

        auto actionGroup = new QActionGroup(this);
        actionGroup->setExclusive(true);
        actionGroup->addAction(actionPerspective);
        actionGroup->addAction(actionOrthogonal);
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
