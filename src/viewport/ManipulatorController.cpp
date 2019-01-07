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

    connect(manipulator.get(), &Manipulator::scaleStarted, this, &ManipulatorController::onScaleStarted);
    connect(manipulator.get(), &Manipulator::scaleChanged, this, &ManipulatorController::onScaleChanged);
    connect(manipulator.get(), &Manipulator::scaleFinished, this, &ManipulatorController::onScaleFinished);

    connect(manipulator.get(), &Manipulator::rotateStarted, this, &ManipulatorController::onRotateStarted);
    connect(manipulator.get(), &Manipulator::rotateChanged, this, &ManipulatorController::onRotateChanged);
    connect(manipulator.get(), &Manipulator::rotateFinished, this, &ManipulatorController::onRotateFinished);
}

glm::dvec3 ManipulatorController::position() const {
    return _item->location().position;
}

void ManipulatorController::onTranslateStarted() {
    _appState->document()->history()->beginChange(tr("Move Item"));
    _initialLocation = _item->location();
}

void ManipulatorController::onTranslateChanged(int axis, double offset) {
    auto loc = _initialLocation;
    loc.position[axis] += offset;
    _item->setLocation(loc);
}

void ManipulatorController::onTranslateFinished() {
}

void ManipulatorController::onScaleStarted() {
    _appState->document()->history()->beginChange(tr("Scale Item"));
    _initialLocation = _item->location();
}

void ManipulatorController::onScaleChanged(int axis, double offset) {
    auto loc = _initialLocation;
    loc.scale[axis] *= offset;
    _item->setLocation(loc);
}

void ManipulatorController::onScaleFinished() {
}

void ManipulatorController::onRotateStarted() {
    _appState->document()->history()->beginChange(tr("Rotate Item"));
    _initialLocation = _item->location();
}

void ManipulatorController::onRotateChanged(int axis, double offset) {
    // TODO: Use Quaternion
    auto loc = _initialLocation;
    loc.rotation[axis] += offset;
    _item->setLocation(loc);
}

void ManipulatorController::onRotateFinished() {
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
