#pragma once
#include "SpinBox.hpp"
#include <QWidget>
#include <vector>

namespace Lattice {
namespace Widget {

class MultiValueSpinBox : public QWidget {
    Q_OBJECT
  public:
    MultiValueSpinBox(QWidget *parent = nullptr);

    auto spinBox() const { return _spinBox; }

    void setValues(const std::vector<int> &values);

  signals:
    void editingFinished(int value);

  private:
    SpinBox *_spinBox;
};

} // namespace Widget
} // namespace Lattice
