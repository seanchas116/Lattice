#include "ItemPropertyPaneViewModel.hpp"

namespace Lattice {

ItemPropertyPaneViewModel::ItemPropertyPaneViewModel(QObject *parent) : QObject(parent)
{
}

void ItemPropertyPaneViewModel::setPositionX(float positionX) {
    _positionX = positionX;
}

void ItemPropertyPaneViewModel::setPositionY(float positionY) {
    _positionY = positionY;
}

void ItemPropertyPaneViewModel::setPositionZ(float positionZ) {
    _positionZ = positionZ;
}

void ItemPropertyPaneViewModel::setHasPositionX(bool hasPositionX) {
    _hasPositionX = hasPositionX;
}

void ItemPropertyPaneViewModel::setHasPositionY(bool hasPositionY) {
    _hasPositionY = hasPositionY;
}

void ItemPropertyPaneViewModel::setHasPositionZ(bool hasPositionZ) {
    _hasPositionZ = hasPositionZ;
}

} // namespace Lattice
