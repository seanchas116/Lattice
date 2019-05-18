#include "MultiValueCheckBox.hpp"
#include <QVBoxLayout>
#include <QCheckBox>

namespace Lattice {
namespace Widget {

MultiValueCheckBox::MultiValueCheckBox(const QString &title, QWidget *parent) : QWidget(parent) {
    auto layout = new QVBoxLayout();
    layout->setMargin(0);
    _checkBox = new QCheckBox(title);
    _checkBox->setTristate(true);
    connect(_checkBox, &QCheckBox::toggled, this, &MultiValueCheckBox::toggled);
    layout->addWidget(_checkBox);
    setLayout(layout);
}

void MultiValueCheckBox::setValues(const std::vector<bool> &values) {
    if (values.empty()) {
        return;
    }

    bool isValuesSame= true;
    bool value = values[0];

    for (size_t i = 1; i < values.size(); ++i) {
        if (value != values[i]) {
            isValuesSame = false;
            break;
        }
    }

    if (isValuesSame) {
        _checkBox->setCheckState(value ? Qt::Checked : Qt::Unchecked);
    } else {
        _checkBox->setCheckState(Qt::PartiallyChecked);
    }
}

} // namespace Widget
} // namespace Lattice
