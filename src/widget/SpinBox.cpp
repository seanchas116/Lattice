#include "SpinBox.hpp"

namespace Lattice {
namespace Widget {

SpinBox::SpinBox(QWidget *parent) : QSpinBox(parent) {
}

void SpinBox::stepBy(int steps) {
    QSpinBox::stepBy(steps);
    emit editingFinished();
}

} // namespace Widget
} // namespace Lattice
