#include "ManipulatorController.hpp"
#include "Manipulator.hpp"
#include "../app/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"

namespace Lattice {

ManipulatorController::ManipulatorController(const SP<Manipulator>& manipulator, const SP<AppState> &appState) :
    _manipulator(manipulator),
    _appState(appState)
{
    connectToItem(appState->document()->currentItem());

    manipulator->setTargetPosition(position());
    connect(this, &ManipulatorController::positionChanged, manipulator.get(), &Manipulator::setTargetPosition);
    connect(manipulator.get(), &Manipulator::onDragStart, this, &ManipulatorController::onDragStart);
    connect(manipulator.get(), &Manipulator::onDrag, this, &ManipulatorController::onDrag);
    connect(manipulator.get(), &Manipulator::onDragEnd, this, &ManipulatorController::onDragEnd);
}

glm::vec3 ManipulatorController::position() const {
    return _item->location().position;
}

void ManipulatorController::onDragStart() {
    _initialPosition = _item->location().position;
}

void ManipulatorController::onDrag(glm::vec3 offset) {
    auto loc = _item->location();
    loc.position = _initialPosition + offset;
    _item->setLocation(loc);
}

void ManipulatorController::onDragEnd() {
    // TODO
}

void ManipulatorController::connectToItem(const SP<Item> &item) {
    disconnect(_connection);
    _item = item;
    auto itemPtr = item.get();
    _connection = connect(itemPtr, &Item::locationChanged, this, [this] {
        emit positionChanged(position());
    });
    emit positionChanged(position());
}

} // namespace Lattice
