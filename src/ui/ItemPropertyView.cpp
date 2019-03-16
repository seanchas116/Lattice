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
            spinBox->setSpecialValueText(" ");
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
    if (_items.empty()) {
        return;
    }

    auto location = (*_items.begin())->location();

    glm::bvec3 isPositionSame {true};
    glm::bvec3 isScaleSame {true};
    glm::bvec3 isRotationSame {true};

    for (auto& item : _items) {
        auto otherLocation = item->location();
        for (int i = 0; i < 3; ++i) {
            if (location.position[i] != otherLocation.position[i]) {
                isPositionSame[i] = false;
            }
            if (location.scale[i] != otherLocation.scale[i]) {
                isScaleSame[i] = false;
            }
            if (glm::eulerAngles(location.rotation)[i] != glm::eulerAngles(otherLocation.rotation)[i]) {
                isRotationSame[i] = false;
            }
        }
    }

    glm::dvec3 eulerAngles = glm::eulerAngles(location.rotation);

    auto specialValue = -std::numeric_limits<double>::infinity();

    for (size_t i = 0; i < 3; ++i) {
        _positionSpinBoxes[i]->setValue(isPositionSame[i] ? location.position[i] : specialValue);
        _scaleSpinBoxes[i]->setValue(isScaleSame[i] ? location.scale[i] : specialValue);
        _rotationSpinBoxes[i]->setValue(isScaleSame[i] ? glm::degrees(eulerAngles[i]) : specialValue);
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
