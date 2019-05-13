#include "SpinBox.hpp"

namespace Lattice {
namespace Widget {

SpinBox::SpinBox(QWidget *parent) : QSpinBox(parent) {
}

void SpinBox::stepBy(int steps) {
    QSpinBox::stepBy(steps);
    emit stepped();
}

} // namespace Widget
} // namespace Lattice
