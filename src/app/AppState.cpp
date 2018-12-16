#include "AppState.hpp"
#include "../document/Document.hpp"

namespace Lattice {

AppState::AppState() {
    _document = std::make_shared<Document>();
    _document->addInitialItems();
}

} // namespace Lattice
