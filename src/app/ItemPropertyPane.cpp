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

    auto buildVec3SpinBoxes = [this] {
        auto gridLayout = new QGridLayout();

        std::array<QDoubleSpinBox*, 3> spinBoxes = {
            new QDoubleSpinBox(),
            new QDoubleSpinBox(),
            new QDoubleSpinBox(),
        };
        //std::array<QString, 3> labels {"X", "Y", "Z"};

        for (size_t i = 0; i < 3; ++i) {
            auto spinBox = spinBoxes[i];
            spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            spinBox->setMinimum(-std::numeric_limits<double>::infinity());
            spinBox->setMaximum(std::numeric_limits<double>::infinity());
            gridLayout->addWidget(spinBox, 0, int(i));

            connect(spinBox, &QDoubleSpinBox::editingFinished, this, &ItemPropertyPane::setLocation);

            //auto label = new QLabel(labels[i]);
            //label->setAlignment(Qt::AlignHCenter);
            //gridLayout->addWidget(label, 1, int(i));
        }

        return std::tuple(gridLayout, spinBoxes);
    };

    layout->addWidget(new QLabel("Position"));

    auto [positionLayout, positionSpinBoxes] = buildVec3SpinBoxes();
    _positionSpinBoxes = positionSpinBoxes;
    layout->addLayout(positionLayout);

    layout->addWidget(new QLabel("Scale"));

    auto [scaleLayout, scaleSpinBoxes] = buildVec3SpinBoxes();
    _scaleSpinBoxes = scaleSpinBoxes;
    layout->addLayout(scaleLayout);

    layout->addWidget(new QLabel("Rotation"));

    auto [rotationLayout, rotationSpinBoxes] = buildVec3SpinBoxes();
    _rotationSpinBoxes = rotationSpinBoxes;
    layout->addLayout(rotationLayout);

    layout->addStretch();
    setLayout(layout);

    connect(appState->document().get(), &Document::currentItemChanged, this, &ItemPropertyPane::onCurrentItemChanged);
    onCurrentItemChanged();
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
    auto location = currentItem ? currentItem->location() : Location();

    for (size_t i = 0; i < 3; ++i) {
        _positionSpinBoxes[i]->setValue(double(location.position[i]));
        _scaleSpinBoxes[i]->setValue(double(location.scale[i]));
        _rotationSpinBoxes[i]->setValue(double(glm::degrees(location.rotation[i])));
    }
}

void ItemPropertyPane::setLocation() {
    auto item = _appState->document()->currentItem();
    if (!item) {
        return;
    }
    auto location = item->location();

    for (size_t i = 0; i < 3; ++i) {
        location.position[i] = _positionSpinBoxes[i]->value();
        location.scale[i] = _scaleSpinBoxes[i]->value();
        location.rotation[i] = glm::radians(glm::mod(_rotationSpinBoxes[i]->value(), 360.0));
    }

    item->setLocation(location);
}

} // namespace Lattice
