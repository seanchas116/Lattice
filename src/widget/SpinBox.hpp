#pragma once
#include <QSpinBox>

namespace Lattice {
namespace Widget {

class SpinBox : public QSpinBox {
    Q_OBJECT
public:
    explicit SpinBox(QWidget *parent = nullptr);
    void stepBy(int steps) override;
};

} // namespace Widget
} // namespace Lattice

