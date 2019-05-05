#include "MeshEditState.hpp"

namespace Lattice {
namespace State {

void MeshEditState::setTargetObject(Opt<SP<Document::MeshObject>> target) {
    if (_targetObject == target) {
        return;
    }
    _targetObject = std::move(target);
    emit targetObjectChanged(target);
}

} // namespace State
} // namespace Lattice
