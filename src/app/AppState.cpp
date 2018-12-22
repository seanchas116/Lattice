#include "AppState.hpp"
#include "../document/Document.hpp"

namespace Lattice {

AppState::AppState() {
    _document = std::make_shared<Document>();
    _document->addInitialItems();
    _document->setCurrentItem(_document->rootItem()->childItems()[0]);
}

} // namespace Lattice
