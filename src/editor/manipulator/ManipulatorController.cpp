#include "ManipulatorController.hpp"
#include "TranslateManipulator.hpp"
#include "../../ui/AppState.hpp"
#include "../../document/Document.hpp"
#include "../../document/Item.hpp"
#include "../../document/History.hpp"
#include "../../support/OptionalGuard.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

ManipulatorController::ManipulatorController(const SP<UI::AppState> &appState) : _appState(appState)
{
    connectToItem(appState->document()->currentItem());
    connect(appState->document().get(), &Document::Document::currentItemChanged, this, &ManipulatorController::connectToItem);

    for (int axis = 0; axis < 3; ++axis) {
        auto handle = makeShared<TranslateManipulator>(axis);
        handle->setTargetPosition(position());
        connect(this, &ManipulatorController::positionChanged, handle.get(), &TranslateManipulator::setTargetPosition);

        connect(handle.get(), &TranslateManipulator::translateStarted, this, [this] { onTranslateStarted(); });
        connect(handle.get(), &TranslateManipulator::translateChanged, this, [this, axis] (double offset) { onTranslateChanged(axis, offset); });
        connect(handle.get(), &TranslateManipulator::translateFinished, this, [this] { onTranslateFinished(); });
        _translateHandles.push_back(std::move(handle));
    }
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

void ManipulatorController::connectToItem(const std::optional<SP<Document::Item> > &maybeItem) {
    disconnect(_connection);
    _item = maybeItem;
    LATTICE_OPTIONAL_GUARD(item, maybeItem, return;)
    auto itemPtr = item.get();
    _connection = connect(itemPtr, &Document::Item::locationChanged, this, [this] {
        emit positionChanged(position());
    });
    emit positionChanged(position());
}

}
} // namespace Editor
} // namespace Lattice
