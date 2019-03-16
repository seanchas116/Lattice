#include "LocationEdit.hpp"

namespace Lattice {
namespace UI {

LocationEdit::LocationEdit(QWidget *parent) : QWidget(parent) {
}

void LocationEdit::setLocation(const Location &location) {
    if (_location == location) {
        return;
    }
    _location = location;
    emit locationChanged(location);
}

} // namespace UI
} // namespace Lattice
