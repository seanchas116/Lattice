#include "ItemPropertyView.hpp"
#include "../state/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"
#include "../document/History.hpp"
#include "../support/Debug.hpp"
#include "../support/OptionalGuard.hpp"
#include "../widget/DoubleSpinBox.hpp"
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QtDebug>

namespace Lattice::UI {

ItemPropertyView::ItemPropertyView(QWidget *parent) :
    QWidget(parent)
{
    auto layout = new QVBoxLayout();

    auto gridLayout = new QGridLayout();

    std::array<QString, 3> labels {"X", "Y", "Z"};
    for (size_t i = 0; i < 3; ++i) {
        auto label = new QLabel(labels[i]);
        label->setAlignment(Qt::AlignHCenter);
        gridLayout->addWidget(label, 0, int(i + 1));
    }

    auto buildVec3SpinBoxes = [&] (const QString& title, int row) {
        auto label = new QLabel(title);
        gridLayout->addWidget(label, row, 0);

        std::array<Widget::DoubleSpinBox*, 3> spinBoxes = {
            new Widget::DoubleSpinBox(),
            new Widget::DoubleSpinBox(),
            new Widget::DoubleSpinBox(),
        };

        for (size_t i = 0; i < 3; ++i) {
            auto spinBox = spinBoxes[i];
            spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            spinBox->setMinimum(-std::numeric_limits<double>::infinity());
            spinBox->setMaximum(std::numeric_limits<double>::infinity());
            gridLayout->addWidget(spinBox, row, int(i + 1));

            connect(spinBox, &Widget::DoubleSpinBox::editingFinished, this, &ItemPropertyView::handleLocationChange);
            connect(spinBox, &Widget::DoubleSpinBox::stepped, this, &ItemPropertyView::handleLocationChange);
        }

        return spinBoxes;
    };

    _positionSpinBoxes = buildVec3SpinBoxes("Position", 1);
    _rotationSpinBoxes = buildVec3SpinBoxes("Rotation", 2);
    _scaleSpinBoxes = buildVec3SpinBoxes("Scale", 3);

    layout->addLayout(gridLayout);
    layout->addStretch();

    setLayout(layout);
}

void ItemPropertyView::setItems(const std::unordered_set<SP<Document::Item> > &items) {
    if (_items == items) {
        return;
    }
    _items = items;

    Opt<SP<Document::Item>> firstItem;
    if (!items.empty()) {
        firstItem = *items.begin();
    }

    disconnect(_itemConnection);

    if (firstItem) {
        _itemConnection = connect(firstItem->get(), &Document::Item::locationChanged, this, &ItemPropertyView::setLocation);
    }

    setLocation();
}

void ItemPropertyView::setLocation() {
    // TODO: support multiple items
    // TODO: spinboxes must be disbled when no item is selected

    auto location = _items.empty() ? Location() : (*_items.begin())->location();
    _location = location;

    glm::dvec3 eulerAngles = glm::eulerAngles(location.rotation);

    for (size_t i = 0; i < 3; ++i) {
        _positionSpinBoxes[i]->setValue(location.position[i]);
        _scaleSpinBoxes[i]->setValue(location.scale[i]);
        _rotationSpinBoxes[i]->setValue(glm::degrees(eulerAngles[i]));
    }
}

void ItemPropertyView::handleLocationChange() {
    if (_items.empty()) {
        return;
    }
    auto item = *_items.begin();

    Location location;

    glm::dvec3 eulerAngles(0);

    for (size_t i = 0; i < 3; ++i) {
        location.position[i] = _positionSpinBoxes[i]->value();
        location.scale[i] = _scaleSpinBoxes[i]->value();
        eulerAngles[i] = glm::radians(glm::mod(_rotationSpinBoxes[i]->value(), 360.0));
    }
    location.rotation = glm::normalize(glm::dquat(eulerAngles));

    _location = location;
    (*item->document())->history()->beginChange(tr("Move Item"));
    item->setLocation(location);
}

} // namespace Lattice
