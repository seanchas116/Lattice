#include "ItemPropertyPane.hpp"
#include "AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"
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

    _posXSpinBox = new QDoubleSpinBox();
    _posYSpinBox = new QDoubleSpinBox();
    _posZSpinBox = new QDoubleSpinBox();
    for (auto spinBox : {_posXSpinBox, _posYSpinBox, _posZSpinBox}) {
        spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        spinBox->setMinimum(-100);
        spinBox->setMaximum(100);
    }

    connect(appState->document().get(), &Document::currentItemChanged, this, &ItemPropertyPane::onCurrentItemChanged);
    onCurrentItemChanged();

    connect(_posXSpinBox, &QDoubleSpinBox::editingFinished, this, &ItemPropertyPane::setLocation);
    connect(_posYSpinBox, &QDoubleSpinBox::editingFinished, this, &ItemPropertyPane::setLocation);
    connect(_posZSpinBox, &QDoubleSpinBox::editingFinished, this, &ItemPropertyPane::setLocation);

    positionLayout->addWidget(_posXSpinBox, 0, 0);
    positionLayout->addWidget(_posYSpinBox, 0, 1);
    positionLayout->addWidget(_posZSpinBox, 0, 2);

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

void ItemPropertyPane::onCurrentItemChanged() {
    disconnect(_itemConnection);

    auto currentItem = _appState->document()->currentItem();
    if (currentItem) {} {
        _itemConnection = connect(currentItem.get(), &Item::locationChanged, this, &ItemPropertyPane::onLocationChanged);
    }
    onLocationChanged();
}

void ItemPropertyPane::onLocationChanged() {
    // TODO: support multiple items
    // TODO: spinboxes must be disbled when no item is selected

    auto currentItem = _appState->document()->currentItem();
    auto pos = currentItem ? currentItem->location().position : glm::vec3(0);

    _posXSpinBox->setValue(double(pos.x));
    _posYSpinBox->setValue(double(pos.y));
    _posZSpinBox->setValue(double(pos.z));
}

void ItemPropertyPane::setLocation() {
    auto item = _appState->document()->currentItem();
    if (!item) {
        return;
    }
    auto location = item->location();

    auto x = _posXSpinBox->value();
    auto y = _posYSpinBox->value();
    auto z = _posZSpinBox->value();
    location.position = glm::vec3(x, y, z);

    item->setLocation(location);
}

} // namespace Lattice
