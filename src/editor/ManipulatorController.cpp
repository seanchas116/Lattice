#include "ManipulatorController.hpp"
#include "Manipulator.hpp"
#include "../ui/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"
#include "../document/History.hpp"
#include "../support/OptionalGuard.hpp"

namespace Lattice::Editor {

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
    LATTICE_OPTIONAL_GUARD(item, _item, return glm::dvec3(0);)
    return item->location().position;
}

void ManipulatorController::onTranslateStarted() {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    _appState->document()->history()->beginChange(tr("Move Item"));
    _initialLocation = item->location();
}

void ManipulatorController::onTranslateChanged(int axis, double offset) {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    auto loc = _initialLocation;
    loc.position[axis] += offset;
    item->setLocation(loc);
}

void ManipulatorController::onTranslateFinished() {
}

void ManipulatorController::onScaleStarted() {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    _appState->document()->history()->beginChange(tr("Scale Item"));
    _initialLocation = item->location();
}

void ManipulatorController::onScaleChanged(int axis, double offset) {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    auto loc = _initialLocation;
    loc.scale[axis] *= offset;
    item->setLocation(loc);
}

void ManipulatorController::onScaleFinished() {
}

void ManipulatorController::onRotateStarted() {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    _appState->document()->history()->beginChange(tr("Rotate Item"));
    _initialLocation = item->location();
}

void ManipulatorController::onRotateChanged(int axis, double offset) {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    auto loc = _initialLocation;

    glm::dvec3 eulerAngles(0);
    eulerAngles[axis] = offset;

    loc.rotation = glm::dquat(eulerAngles) * loc.rotation;
    item->setLocation(loc);
}

void ManipulatorController::onRotateFinished() {
}

void ManipulatorController::connectToItem(const std::optional<SP<Document::Item>> &maybeItem) {
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
