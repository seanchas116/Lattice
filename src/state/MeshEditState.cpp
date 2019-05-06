#include "MeshEditState.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../mesh/Mesh.hpp"

namespace Lattice {
namespace State {

MeshEditState::MeshEditState(const SP<Document::MeshObject> &targetObject) :
    _targetObject(targetObject),
    _mesh(makeShared<Mesh::Mesh>(targetObject->mesh())) {

    connect(targetObject.get(), &Document::MeshObject::meshChanged, this, [this](auto& mesh) {
        *_mesh = mesh;
        notifyMeshChange();
    });
}

void MeshEditState::setMesh(Mesh::Mesh mesh) {
    *_mesh = std::move(mesh);
    emit meshChanged(*_mesh);
}

void MeshEditState::notifyMeshChange() {
    emit meshChanged(*_mesh);
}

void MeshEditState::commitMeshChange(const QString &changeTitle) {
    auto maybeDocument = _targetObject->document();
    if (!maybeDocument) {
        return;
    }
    auto document = *maybeDocument;

    _mesh->collectGarbage();
    document->history()->beginChange(changeTitle);
    _targetObject->setMesh(*_mesh);
}

} // namespace State
} // namespace Lattice
