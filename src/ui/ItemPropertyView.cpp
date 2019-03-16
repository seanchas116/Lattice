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

    auto buildVec3SpinBoxes = [&] (LocationMember member, const QString& title, int row) {
        auto label = new QLabel(title);
        gridLayout->addWidget(label, row, 0);

        std::array<Widget::DoubleSpinBox*, 3> spinBoxes = {
            new Widget::DoubleSpinBox(),
            new Widget::DoubleSpinBox(),
            new Widget::DoubleSpinBox(),
        };

        for (int i = 0; i < 3; ++i) {
            auto spinBox = spinBoxes[i];
            spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            spinBox->setMinimum(-std::numeric_limits<double>::infinity());
            spinBox->setMaximum(std::numeric_limits<double>::infinity());
            spinBox->setSpecialValueText(" ");
            gridLayout->addWidget(spinBox, row, int(i + 1));

            auto handleValueChange = [this, spinBox, member, i] {
                this->handleLocationValueChange(member, i, spinBox->value());
            };
            connect(spinBox, &Widget::DoubleSpinBox::editingFinished, this, handleValueChange);
            connect(spinBox, &Widget::DoubleSpinBox::stepped, this, handleValueChange);
        }

        return spinBoxes;
    };

    _positionSpinBoxes = buildVec3SpinBoxes(LocationMember::Position, "Position", 1);
    _rotationSpinBoxes = buildVec3SpinBoxes(LocationMember::Rotation, "Rotation", 2);
    _scaleSpinBoxes = buildVec3SpinBoxes(LocationMember::Scale, "Scale", 3);

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
        _rotationSpinBoxes[i]->setValue(isRotationSame[i] ? glm::degrees(eulerAngles[i]) : specialValue);
    }
}

void ItemPropertyView::handleLocationValueChange(LocationMember member, int index, double value) {
    if (_items.empty()) {
        return;
    }
    auto specialValue = -std::numeric_limits<double>::infinity();
    if (value == specialValue) {
        return;
    }

    (*(*_items.begin())->document())->history()->beginChange(tr("Move Item"));
    for (auto& item : _items) {
        auto location = item->location();
        auto eulerAngles = glm::eulerAngles(location.rotation);

        switch (member) {
        case LocationMember::Position:
            location.position[index] = value;
            break;
        case LocationMember::Scale:
            location.scale[index] = value;
            break;
        case LocationMember::Rotation:
            eulerAngles[index] = glm::radians(glm::mod(value, 360.0));
            break;
        }
        location.rotation = glm::normalize(glm::dquat(eulerAngles));
        item->setLocation(location);
    }
}

} // namespace Lattice
