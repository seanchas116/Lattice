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

    layout->addWidget(new QLabel("<b>Position</b>"));

    {
        auto positionLayout = new QGridLayout();

        _positionSpinBoxes = {
            new QDoubleSpinBox(),
            new QDoubleSpinBox(),
            new QDoubleSpinBox(),
        };
        std::array<QString, 3> labels {"X", "Y", "Z"};

        for (size_t i = 0; i < 3; ++i) {
            auto spinBox = _positionSpinBoxes[i];
            spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            spinBox->setMinimum(-std::numeric_limits<double>::infinity());
            spinBox->setMaximum(std::numeric_limits<double>::infinity());
            connect(spinBox, &QDoubleSpinBox::editingFinished, this, &ItemPropertyPane::setLocation);
            positionLayout->addWidget(spinBox, 0, int(i));

            auto label = new QLabel(labels[i]);
            label->setAlignment(Qt::AlignHCenter);
            positionLayout->addWidget(label, 1, int(i));
        }

        layout->addLayout(positionLayout);
    }

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
    auto pos = currentItem ? currentItem->location().position : glm::vec3(0);

    _positionSpinBoxes[0]->setValue(double(pos.x));
    _positionSpinBoxes[1]->setValue(double(pos.y));
    _positionSpinBoxes[2]->setValue(double(pos.z));
}

void ItemPropertyPane::setLocation() {
    auto item = _appState->document()->currentItem();
    if (!item) {
        return;
    }
    auto location = item->location();

    auto x = _positionSpinBoxes[0]->value();
    auto y = _positionSpinBoxes[1]->value();
    auto z = _positionSpinBoxes[2]->value();
    location.position = glm::vec3(x, y, z);

    item->setLocation(location);
}

} // namespace Lattice
