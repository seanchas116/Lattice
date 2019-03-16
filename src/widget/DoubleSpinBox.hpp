#pragma once
#include <QDoubleSpinBox>

namespace Lattice {
namespace Widget {

class DoubleSpinBox : public QDoubleSpinBox {
    Q_OBJECT
public:
    explicit DoubleSpinBox(QWidget *parent = nullptr);
    void stepBy(int steps) override;

signals:
    void stepped();
};

} // namespace Widget
} // namespace Lattice

