#include "MeshEditState.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../mesh/Mesh.hpp"

namespace Lattice {
namespace State {

MeshEditState::MeshEditState() :
    _mesh(makeShared<Mesh::Mesh>()) {
}

void MeshEditState::setTargetObject(Opt<SP<Document::MeshObject>> target) {
    if (_targetObject == target) {
        return;
    }
    _targetObject = std::move(target);
    emit targetObjectChanged(target);
}

void MeshEditState::setMesh(Mesh::Mesh mesh) {
    *_mesh = std::move(mesh);
    emit meshChanged(*_mesh);
}

void MeshEditState::notifyMeshChange() {
    emit meshChanged(*_mesh);
}

void MeshEditState::commitMeshChange(const QString &changeTitle) {
    if (!_targetObject) {
        return;
    }
    auto object = *_targetObject;

    auto maybeDocument = object->document();
    if (!maybeDocument) {
        return;
    }
    auto document = *maybeDocument;

    _mesh->collectGarbage();
    document->history()->beginChange(changeTitle);
    object->setMesh(*_mesh);
}

} // namespace State
} // namespace Lattice
