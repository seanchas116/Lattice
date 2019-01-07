#include "ManipulatorController.hpp"
#include "Manipulator.hpp"
#include "../ui/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"
#include "../document/History.hpp"

namespace Lattice::Viewport {

ManipulatorController::ManipulatorController(const SP<Manipulator>& manipulator, const SP<UI::AppState> &appState) :
    _manipulator(manipulator),
    _appState(appState)
{
    connectToItem(appState->document()->currentItem());
    connect(appState->document().get(), &Document::Document::currentItemChanged, this, &ManipulatorController::connectToItem);

    manipulator->setTargetPosition(position());
    connect(this, &ManipulatorController::positionChanged, manipulator.get(), &Manipulator::setTargetPosition);
    connect(manipulator.get(), &Manipulator::translateStarted, this, &ManipulatorController::onTranslateStarted);
    connect(manipulator.get(), &Manipulator::translateChanged, this, &ManipulatorController::onTranslateChanged);
    connect(manipulator.get(), &Manipulator::translateFinished, this, &ManipulatorController::onTranslateFinished);
}

glm::dvec3 ManipulatorController::position() const {
    return _item->location().position;
}

void ManipulatorController::onTranslateStarted() {
    _appState->document()->history()->beginChange(tr("Move Item"));
    _initialPosition = _item->location().position;
}

void ManipulatorController::onTranslateChanged(glm::dvec3 offset) {
    auto loc = _item->location();
    loc.position = _initialPosition + offset;
    _item->setLocation(loc);
}

void ManipulatorController::onTranslateFinished() {
}

void ManipulatorController::connectToItem(const SP<Document::Item> &item) {
    disconnect(_connection);
    _item = item;
    auto itemPtr = item.get();
    _connection = connect(itemPtr, &Document::Item::locationChanged, this, [this] {
        emit positionChanged(position());
    });
    emit positionChanged(position());
}

} // namespace Lattice
