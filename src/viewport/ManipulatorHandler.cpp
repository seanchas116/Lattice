#include "ManipulatorHandler.hpp"
#include "../app/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"

namespace Lattice {

ManipulatorHandler::ManipulatorHandler(const SP<AppState> &appState) : _appState(appState) {
    connectToItem(appState->document()->currentItem());
}

glm::vec3 ManipulatorHandler::position() const {
    return _item->location().position;
}

void ManipulatorHandler::onMoveStart() {
    // TODO
}

void ManipulatorHandler::onMove(glm::vec3 offset) {
    // TODO
}

void ManipulatorHandler::onMoveEnd() {
    // TODO
}

void ManipulatorHandler::connectToItem(const SP<Item> &item) {
    disconnect(_connection);
    _item = item;
    auto itemPtr = item.get();
    _connection = connect(itemPtr, &Item::locationChanged, this, [this] {
        emit positionChanged(position());
    });
    emit positionChanged(position());
}

} // namespace Lattice
