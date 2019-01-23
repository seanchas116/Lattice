#include "ViewportControlView.hpp"
#include <QRadioButton>
#include <QVBoxLayout>

namespace Lattice {
namespace Editor {

ViewportControlView::ViewportControlView(QWidget *parent) : QWidget(parent) {
    auto layout = new QVBoxLayout();

    {

        auto buttonPerspective = new QRadioButton(tr("Perspective"));
        buttonPerspective->setAutoExclusive(true);

        auto buttonOrthogonal = new QRadioButton(tr("Orthogonal"));
        buttonOrthogonal->setAutoExclusive(true);

        auto radioButtonsLayout = new QVBoxLayout();
        radioButtonsLayout->addWidget(buttonPerspective);
        radioButtonsLayout->addWidget(buttonOrthogonal);

        auto radioButtons = new QWidget();
        radioButtons->setLayout(radioButtonsLayout);
        layout->addWidget(radioButtons);
    }

    setLayout(layout);
}

} // namespace Editor
} // namespace Lattice
