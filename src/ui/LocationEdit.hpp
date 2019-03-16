#pragma once

#include <QWidget>
#include "../support/Location.hpp"

namespace Lattice {
namespace UI {

class LocationEdit : public QWidget {
    Q_OBJECT
public:
    explicit LocationEdit(QWidget *parent = nullptr);

    void setLocation(const Location& location);
    auto& location() const { return _location; }

signals:
    void locationChanged(const Location& location);

private:
    Location _location;
};

} // namespace UI
} // namespace Lattice
