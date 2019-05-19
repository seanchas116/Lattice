#pragma once
#include <QWidget>
#include <vector>
#include "DoubleSpinBox.hpp"

namespace Lattice {
namespace Widget {

class MultiValueDoubleSpinBox : public QWidget {
    Q_OBJECT
public:
    MultiValueDoubleSpinBox(QWidget* parent = nullptr);

    auto doubleSpinBox() const { return _doubleSpinBox; }

    void setValues(const std::vector<double>& values);

signals:
    void editingFinished(double value);

private:
    DoubleSpinBox* _doubleSpinBox;
};

} // namespace Widget
} // namespace Lattice
