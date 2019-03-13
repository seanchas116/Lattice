#include "ItemPropertyView.hpp"
#include "../state/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"
#include "../document/History.hpp"
#include "../support/Debug.hpp"
#include "../support/OptionalGuard.hpp"
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QtDebug>

namespace Lattice::UI {

ItemPropertyView::ItemPropertyView(const SP<State::AppState> &appState, QWidget *parent) :
    QWidget(parent), _appState(appState)
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

        std::array<QDoubleSpinBox*, 3> spinBoxes = {
            new QDoubleSpinBox(),
            new QDoubleSpinBox(),
            new QDoubleSpinBox(),
        };

        for (size_t i = 0; i < 3; ++i) {
            auto spinBox = spinBoxes[i];
            spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            spinBox->setMinimum(-std::numeric_limits<double>::infinity());
            spinBox->setMaximum(std::numeric_limits<double>::infinity());
            gridLayout->addWidget(spinBox, row, int(i + 1));

            connect(spinBox, &QDoubleSpinBox::editingFinished, this, &ItemPropertyView::setLocation);
        }

        return spinBoxes;
    };

    _positionSpinBoxes = buildVec3SpinBoxes("Position", 1);
    _rotationSpinBoxes = buildVec3SpinBoxes("Rotation", 2);
    _scaleSpinBoxes = buildVec3SpinBoxes("Scale", 3);

    layout->addLayout(gridLayout);
    layout->addStretch();

    setLayout(layout);

    connect(appState->document().get(), &Document::Document::currentItemChanged, this, &ItemPropertyView::onCurrentItemChanged);
    onCurrentItemChanged();
}

void ItemPropertyView::onCurrentItemChanged() {
    disconnect(_itemConnection);

    auto currentItem = _appState->document()->currentItem();
    if (currentItem) {
        _itemConnection = connect(currentItem->get(), &Document::Item::locationChanged, this, &ItemPropertyView::onLocationChanged);
        setEnabled(true);
    } else {
        setEnabled(false);
    }
    onLocationChanged();
}

void ItemPropertyView::onLocationChanged() {
    // TODO: support multiple items
    // TODO: spinboxes must be disbled when no item is selected

    auto currentItem = _appState->document()->currentItem();
    auto location = currentItem ? (*currentItem)->location() : Location();
    _location = location;

    glm::dvec3 eulerAngles = glm::eulerAngles(location.rotation);

    for (size_t i = 0; i < 3; ++i) {
        _positionSpinBoxes[i]->setValue(location.position[i]);
        _scaleSpinBoxes[i]->setValue(location.scale[i]);
        _rotationSpinBoxes[i]->setValue(glm::degrees(eulerAngles[i]));
    }
}

void ItemPropertyView::setLocation() {
    LATTICE_OPTIONAL_GUARD(item,  _appState->document()->currentItem(), return;)
    Location location;

    glm::dvec3 eulerAngles(0);

    for (size_t i = 0; i < 3; ++i) {
        location.position[i] = _positionSpinBoxes[i]->value();
        location.scale[i] = _scaleSpinBoxes[i]->value();
        eulerAngles[i] = glm::radians(glm::mod(_rotationSpinBoxes[i]->value(), 360.0));
    }
    location.rotation = glm::normalize(glm::dquat(eulerAngles));

    _location = location;
    _appState->document()->history()->beginChange(tr("Move Item"));
    item->setLocation(location);
}

void ItemPropertyView::setEnabled(bool enabled) {
    for (auto& sb : _positionSpinBoxes) {
        sb->setEnabled(enabled);
    }
    for (auto& sb : _rotationSpinBoxes) {
        sb->setEnabled(enabled);
    }
    for (auto& sb : _scaleSpinBoxes) {
        sb->setEnabled(enabled);
    }
}

} // namespace Lattice
