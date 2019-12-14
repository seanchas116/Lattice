#include "MultiValueDoubleSpinBox.hpp"
#include <QVBoxLayout>

namespace Lattice {
namespace Widget {

MultiValueDoubleSpinBox::MultiValueDoubleSpinBox(QWidget *parent) : QWidget(parent) {
    auto layout = new QVBoxLayout();
    layout->setMargin(0);

    _doubleSpinBox = new DoubleSpinBox();
    _doubleSpinBox->setMinimum(-std::numeric_limits<double>::infinity());
    _doubleSpinBox->setMaximum(std::numeric_limits<double>::infinity());
    _doubleSpinBox->setSpecialValueText(" ");

    connect(_doubleSpinBox, &DoubleSpinBox::editingFinished, this, [this] {
        auto specialValue = _doubleSpinBox->minimum();
        auto value = _doubleSpinBox->value();
        if (value == specialValue) {
            return;
        }
        emit editingFinished(value);
    });
    layout->addWidget(_doubleSpinBox);
    setLayout(layout);
}

void MultiValueDoubleSpinBox::setValues(const std::vector<double> &values) {
    if (values.empty()) {
        return;
    }

    bool isValuesSame = true;
    double value = values[0];

    for (size_t i = 1; i < values.size(); ++i) {
        if (value != values[i]) {
            isValuesSame = false;
            break;
        }
    }

    if (isValuesSame) {
        _doubleSpinBox->setValue(value);
    } else {
        _doubleSpinBox->setValue(_doubleSpinBox->minimum());
    }
}

} // namespace Widget
} // namespace Lattice
