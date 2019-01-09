#include "Controller.hpp"
#include "TranslateHandle.hpp"
#include "ScaleHandle.hpp"
#include "../../ui/AppState.hpp"
#include "../../document/Document.hpp"
#include "../../document/Item.hpp"
#include "../../document/History.hpp"
#include "../../support/OptionalGuard.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

Controller::Controller(const SP<UI::AppState> &appState) : _appState(appState)
{
    connectToItem(appState->document()->currentItem());
    connect(appState->document().get(), &Document::Document::currentItemChanged, this, &Controller::connectToItem);

    for (int axis = 0; axis < 3; ++axis) {
        auto handle = makeShared<TranslateHandle>(axis);
        handle->setTargetPosition(position());
        connect(this, &Controller::positionChanged, handle.get(), &TranslateHandle::setTargetPosition);

        connect(handle.get(), &TranslateHandle::translateStarted, this, [this] { onTranslateStarted(); });
        connect(handle.get(), &TranslateHandle::translateChanged, this, [this, axis] (double offset) { onTranslateChanged(axis, offset); });
        connect(handle.get(), &TranslateHandle::translateFinished, this, [this] { onTranslateFinished(); });
        _translateHandles.push_back(std::move(handle));
    }

    for (int axis = 0; axis < 3; ++axis) {
        auto handle = makeShared<ScaleHandle>(axis);
        handle->setTargetPosition(position());
        connect(this, &Controller::positionChanged, handle.get(), &ScaleHandle::setTargetPosition);

        connect(handle.get(), &ScaleHandle::scaleStarted, this, [this] { onScaleStarted(); });
        connect(handle.get(), &ScaleHandle::scaleChanged, this, [this, axis] (double offset) { onScaleChanged(axis, offset); });
        connect(handle.get(), &ScaleHandle::scaleFinished, this, [this] { onScaleFinished(); });
        _scaleHandles.push_back(std::move(handle));
    }
}

glm::dvec3 Controller::position() const {
    LATTICE_OPTIONAL_GUARD(item, _item, return glm::dvec3(0);)
    return item->location().position;
}

void Controller::onTranslateStarted() {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    _appState->document()->history()->beginChange(tr("Move Item"));
    _initialLocation = item->location();
}

void Controller::onTranslateChanged(int axis, double offset) {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    auto loc = _initialLocation;
    loc.position[axis] += offset;
    item->setLocation(loc);
}

void Controller::onTranslateFinished() {
}

void Controller::onScaleStarted() {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    _appState->document()->history()->beginChange(tr("Scale Item"));
    _initialLocation = item->location();
}

void Controller::onScaleChanged(int axis, double offset) {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    auto loc = _initialLocation;
    loc.scale[axis] *= offset;
    item->setLocation(loc);
}

void Controller::onScaleFinished() {
}

void Controller::onRotateStarted() {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    _appState->document()->history()->beginChange(tr("Rotate Item"));
    _initialLocation = item->location();
}

void Controller::onRotateChanged(int axis, double offset) {
    LATTICE_OPTIONAL_GUARD(item, _item, return;)

    auto loc = _initialLocation;

    glm::dvec3 eulerAngles(0);
    eulerAngles[axis] = offset;

    loc.rotation = glm::dquat(eulerAngles) * loc.rotation;
    item->setLocation(loc);
}

void Controller::onRotateFinished() {
}

void Controller::connectToItem(const std::optional<SP<Document::Item> > &maybeItem) {
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
