#include "MultiValueSpinBox.hpp"
#include <QVBoxLayout>

namespace Lattice {
namespace Widget {

MultiValueSpinBox::MultiValueSpinBox(QWidget *parent) : QWidget(parent) {
    auto layout = new QVBoxLayout();
    layout->setMargin(0);

    _spinBox = new SpinBox();
    _spinBox->setMinimum(std::numeric_limits<int>::min());
    _spinBox->setMaximum(std::numeric_limits<int>::max());
    _spinBox->setSpecialValueText(" ");

    connect(_spinBox, &SpinBox::editingFinished, this, [this] {
        auto specialValue = _spinBox->minimum();
        auto value = _spinBox->value();
        if (value == specialValue) {
            return;
        }
        emit editingFinished(value);
    });
    layout->addWidget(_spinBox);
    setLayout(layout);
}

void MultiValueSpinBox::setValues(const std::vector<int> &values) {
    if (values.empty()) {
        return;
    }

    bool isValuesSame= true;
    int value = values[0];

    for (size_t i = 1; i < values.size(); ++i) {
        if (value != values[i]) {
            isValuesSame = false;
            break;
        }
    }

    if (isValuesSame) {
        _spinBox->setValue(value);
    } else {
        _spinBox->setValue(_spinBox->minimum());
    }
}

} // namespace Widget
} // namespace Lattice
