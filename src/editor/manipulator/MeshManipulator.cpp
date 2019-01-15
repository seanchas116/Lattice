#include "MeshManipulator.hpp"
#include "../../ui/AppState.hpp"
#include "../../document/Document.hpp"
#include "../../document/MeshItem.hpp"
#include "../../document/History.hpp"

namespace Lattice {
namespace Editor {
namespace Manipulator {

MeshManipulator::MeshManipulator(const SP<UI::AppState> &appState) : _appState(appState)
{
    connectToItem(appState->document()->editedItem());
    connect(appState->document().get(), &Document::Document::editedItemChanged, this, &MeshManipulator::connectToItem);
    connect(appState->document().get(), &Document::Document::meshSelectionChanged, this, &MeshManipulator::updatePosition);
    connect(this, &Manipulator::onBegin, this, &MeshManipulator::handleOnBegin);
    connect(this, &Manipulator::onChange, this, &MeshManipulator::handleOnChange);
    connect(this, &Manipulator::onEnd, this, &MeshManipulator::handleOnEnd);
}

void MeshManipulator::handleOnBegin(ValueType type, double value) {
    // TODO
    LATTICE_OPTIONAL_GUARD(item, _item, return;)
}

void MeshManipulator::handleOnChange(ValueType type, int axis, double value) {
    // TODO
    LATTICE_OPTIONAL_GUARD(item, _item, return;)
}

void MeshManipulator::handleOnEnd(ValueType type) {
    Q_UNUSED(type);
}

void MeshManipulator::connectToItem(const std::optional<SP<Document::MeshItem> > &maybeItem) {
    disconnect(_connection);
    _item = maybeItem;
    LATTICE_OPTIONAL_GUARD(item, maybeItem, return;)
    auto itemPtr = item.get();
    _connection = connect(itemPtr, &Document::Item::locationChanged, this, [this] {
        updatePosition();
    });
    updatePosition();
}

void MeshManipulator::updatePosition() {
    setTargetPosition(_appState->document()->meshSelection().medianPosition());
}

}
} // namespace Editor
} // namespace Lattice
