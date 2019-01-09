#include "OldManipulatorController.hpp"
#include "Manipulator.hpp"
#include "../ui/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"
#include "../document/History.hpp"
#include "../support/OptionalGuard.hpp"

namespace Lattice::Editor {

OldManipulatorController::OldManipulatorController(const SP<Manipulator>& manipulator, const SP<UI::AppState> &appState) :
    _manipulator(manipulator),
    _appState(appState)
{
    connectToItem(appState->document()->currentItem());
    connect(appState->document().get(), &Document::Document::currentItemChanged, this, &OldManipulatorController::connectToItem);

    manipulator->setTargetPosition(position());
    connect(this, &OldManipulatorController::positionChanged, manipulator.get(), &Manipulator::setTargetPosition);

    connect(manipulator.get(), &Manipulator::translateStarted, this, &OldManipulatorController::onTranslateStarted);
    connect(manipulator.get(), &Manipulator::translateChanged, this, &OldManipulatorController::onTranslateChanged);
    connect(manipulator.get(), &Manipulator::translateFinished, this, &OldManipulatorController::onTranslateFinished);

    connect(manipulator.get(), &Manipulator::scaleStarted, this, &OldManipulatorController::onScaleStarted);
    connect(manipulator.get(), &Manipulator::scaleChanged, this, &OldManipulatorController::onScaleChanged);
    connect(manipulator.get(), &Manipulator::scaleFinished, this, &OldManipulatorController::onScaleFinished);

    connect(manipulator.get(), &Manipulator::rotateStarted, this, &OldManipulatorController::onRotateStarted);
    connect(manipulator.get(), &Manipulator::rotateChanged, this, &OldManipulatorController::onRotateChanged);
    connect(manipulator.get(), &Manipulator::rotateFinished, this, &OldManipulatorController::onRotateFinished);
}

glm::dvec3 OldManipulatorController::position() const {
    LATTICE_OPTIONAL_GUARD(item, _item, return glm::dvec3(0);)
    return item->location().position;
}

void OldManipulatorController::onTranslateStarted() {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    _appState->document()->history()->beginChange(tr("Move Item"));
    _initialLocation = item->location();
}

void OldManipulatorController::onTranslateChanged(int axis, double offset) {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    auto loc = _initialLocation;
    loc.position[axis] += offset;
    item->setLocation(loc);
}

void OldManipulatorController::onTranslateFinished() {
}

void OldManipulatorController::onScaleStarted() {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    _appState->document()->history()->beginChange(tr("Scale Item"));
    _initialLocation = item->location();
}

void OldManipulatorController::onScaleChanged(int axis, double offset) {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    auto loc = _initialLocation;
    loc.scale[axis] *= offset;
    item->setLocation(loc);
}

void OldManipulatorController::onScaleFinished() {
}

void OldManipulatorController::onRotateStarted() {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    _appState->document()->history()->beginChange(tr("Rotate Item"));
    _initialLocation = item->location();
}

void OldManipulatorController::onRotateChanged(int axis, double offset) {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    auto loc = _initialLocation;

    glm::dvec3 eulerAngles(0);
    eulerAngles[axis] = offset;

    loc.rotation = glm::dquat(eulerAngles) * loc.rotation;
    item->setLocation(loc);
}

void OldManipulatorController::onRotateFinished() {
}

void OldManipulatorController::connectToItem(const std::optional<SP<Document::Item>> &maybeItem) {
    disconnect(_connection);
    _item = maybeItem;
    LATTICE_OPTIONAL_GUARD(item, maybeItem, return;)
    auto itemPtr = item.get();
    _connection = connect(itemPtr, &Document::Item::locationChanged, this, [this] {
        emit positionChanged(position());
    });
    emit positionChanged(position());
}

} // namespace Lattice
