#include "ItemPropertyPane.hpp"
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QtDebug>

namespace Lattice {

ItemPropertyPane::ItemPropertyPane(const SP<AppState> &appState, QWidget *parent) :
    QWidget(parent), _appState(appState)
{
    auto layout = new QVBoxLayout();

    auto positionLayout = new QGridLayout();

    auto xSpinBox = new QDoubleSpinBox();
    auto ySpinBox = new QDoubleSpinBox();
    auto zSpinBox = new QDoubleSpinBox();
    for (auto spinBox : {xSpinBox, ySpinBox, zSpinBox}) {
        spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        spinBox->setMinimum(-100);
        spinBox->setMaximum(100);
    }

    /*
    auto updatePosition = [=] {
        auto pos = _editorState->selection().medianPosition();
        xSpinBox->setValue(double(pos.x));
        ySpinBox->setValue(double(pos.y));
        zSpinBox->setValue(double(pos.z));
    };

    connect(editorState.get(), &EditorState::selectionChanged, this, updatePosition);
    // TODO: support current item changes
    connect(editorState->focusedItem().get(), &Item::changed, this, updatePosition);

    auto setPosition = [=] {
        auto x = xSpinBox->value();
        auto y = ySpinBox->value();
        auto z = zSpinBox->value();
        _editorState->selection().setMedianPosition(glm::vec3(x, y, z));
        _editorState->focusedItem()->notifyChanged();
    };
    connect(xSpinBox, &QDoubleSpinBox::editingFinished, this, setPosition);
    connect(ySpinBox, &QDoubleSpinBox::editingFinished, this, setPosition);
    connect(zSpinBox, &QDoubleSpinBox::editingFinished, this, setPosition);
    */

    positionLayout->addWidget(xSpinBox, 0, 0);
    positionLayout->addWidget(ySpinBox, 0, 1);
    positionLayout->addWidget(zSpinBox, 0, 2);

    auto xLabel = new QLabel("X");
    xLabel->setAlignment(Qt::AlignHCenter);
    auto yLabel = new QLabel("Y");
    yLabel->setAlignment(Qt::AlignHCenter);
    auto zLabel = new QLabel("Z");
    zLabel->setAlignment(Qt::AlignHCenter);

    positionLayout->addWidget(xLabel, 1, 0);
    positionLayout->addWidget(yLabel, 1, 1);
    positionLayout->addWidget(zLabel, 1, 2);

    layout->addLayout(positionLayout);

    layout->addStretch();

    setLayout(layout);
}

} // namespace Lattice
