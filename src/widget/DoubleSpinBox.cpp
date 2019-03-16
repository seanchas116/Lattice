#include "DoubleSpinBox.hpp"

namespace Lattice {
namespace Widget {

DoubleSpinBox::DoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent) {
}

void DoubleSpinBox::stepBy(int steps) {
    QDoubleSpinBox::stepBy(steps);
    emit stepped();
}

} // namespace Widget
} // namespace Lattice
