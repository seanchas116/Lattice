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

void MeshEditState::deleteVertices() {
    auto& mesh = *_mesh;
    auto vertices = mesh.selectedVertices() | ranges::to_vector;
    for (auto v : vertices) {
        mesh.removeVertex(v);
    }
    mesh.collectGarbage();
    commitMeshChange(tr("Delete Vertices"));
}

void MeshEditState::deleteEdges() {
    auto& mesh = *_mesh;
    auto edges = mesh.edges(mesh.selectedVertices());
    for (auto e : edges) {
        mesh.removeEdge(e);
    }
    mesh.collectGarbage();
    commitMeshChange(tr("Delete Edges"));
}

void MeshEditState::deleteFaces() {
    auto& mesh = *_mesh;
    auto faces = mesh.faces(mesh.selectedVertices());
    for (auto f : faces) {
        mesh.removeFace(f);
    }
    mesh.collectGarbage();
    commitMeshChange(tr("Delete Faces"));
}

void MeshEditState::selectAll() {
    auto& mesh = *_mesh;
    for (auto v : mesh.vertices()) {
        mesh.setSelected(v, true);
    }
    notifyMeshChange();
}

void MeshEditState::deselectAll() {
    auto& mesh = *_mesh;
    for (auto v : mesh.vertices()) {
        mesh.setSelected(v, false);
    }
    notifyMeshChange();
}

} // namespace State
} // namespace Lattice
