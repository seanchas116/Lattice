#pragma once
#include <QWidget>
#include <vector>

class QCheckBox;

namespace Lattice {
namespace Widget {

class MultiValueCheckBox : public QWidget {
    Q_OBJECT
public:
    MultiValueCheckBox(const QString& title, QWidget* parent = nullptr);

    void setValues(const std::vector<bool>& values);

signals:
    void clicked(bool value);

private:
    QCheckBox* _checkBox;
};

} // namespace Widget
} // namespace Lattice
