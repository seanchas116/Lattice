#include "MeshEditState.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include <meshlib/Mesh.hpp>
#include <meshlib/algorithm/FindBelt.hpp>
#include <meshlib/algorithm/FindConnected.hpp>
#include <meshlib/algorithm/FindLoop.hpp>
#include <meshlib/algorithm/FlipFace.hpp>

namespace Lattice {
namespace State {

MeshEditState::MeshEditState(const SP<Document::MeshObject> &targetObject) : _object(targetObject),
                                                                             _mesh(makeShared<meshlib::Mesh>(targetObject->mesh())) {

    connect(targetObject.get(), &Document::MeshObject::meshChanged, this, [this](auto &mesh) {
        *_mesh = mesh;
        notifyMeshChanged();
    });
}

void MeshEditState::setMesh(meshlib::Mesh mesh) {
    *_mesh = std::move(mesh);
    emit meshChanged(*_mesh);
}

void MeshEditState::notifyMeshChanged() {
    emit meshChanged(*_mesh);
}

void MeshEditState::commitMeshChanged(const QString &changeTitle) {
    auto maybeDocument = _object->document();
    if (!maybeDocument) {
        return;
    }
    auto document = *maybeDocument;

    document->history()->beginChange(changeTitle);
    _object->setMesh(_mesh->collectGarbage());
}

void MeshEditState::deleteVertices() {
    auto &mesh = *_mesh;
    auto vertices = mesh.selectedVertices() | ranges::to_vector;
    for (auto v : vertices) {
        mesh.removeVertex(v);
    }
    commitMeshChanged(tr("Delete Vertices"));
}

void MeshEditState::deleteEdges() {
    auto &mesh = *_mesh;
    auto edges = mesh.edges(mesh.selectedVertices());
    for (auto e : edges) {
        mesh.removeEdge(e);
    }
    commitMeshChanged(tr("Delete Edges"));
}

void MeshEditState::deleteFaces() {
    auto &mesh = *_mesh;
    auto faces = mesh.faces(mesh.selectedVertices());
    for (auto f : faces) {
        mesh.removeFace(f);
    }
    commitMeshChanged(tr("Delete Faces"));
}

void MeshEditState::selectAll() {
    _mesh->selectAll();
    notifyMeshChanged();
}

void MeshEditState::deselectAll() {
    _mesh->deselectAll();
    notifyMeshChanged();
}

void MeshEditState::invertSelection() {
    auto &mesh = *_mesh;
    for (auto v : mesh.vertices()) {
        mesh.setSelected(v, !mesh.isSelected(v));
    }
    notifyMeshChanged();
}

void MeshEditState::selectLoop(meshlib::EdgeHandle edge) {
    auto edges = meshlib::findLoop(*_mesh, edge);

    _mesh->deselectAll();
    for (auto e : edges) {
        for (auto v : _mesh->vertices(e)) {
            _mesh->setSelected(v, true);
        }
    }

    notifyMeshChanged();
}

void MeshEditState::selectBelt(meshlib::EdgeHandle edge) {
    auto belt = findBelt(*_mesh, edge);

    _mesh->deselectAll();
    for (auto [edge, face, isEdgeReverse] : belt) {
        for (auto v : _mesh->vertices(edge)) {
            _mesh->setSelected(v, true);
        }
    }

    notifyMeshChanged();
}

void MeshEditState::selectConnected(const std::vector<meshlib::VertexHandle> &vertices) {
    auto connected = findConnected(*_mesh, vertices);

    _mesh->deselectAll();
    for (auto &&v : connected) {
        _mesh->setSelected(v, true);
    }

    notifyMeshChanged();
}

void MeshEditState::flipFaces() {
    auto selectedFaces = _mesh->selectedFaces();
    for (auto &&face : selectedFaces) {
        meshlib::flipFace(*_mesh, face);
    }

    notifyMeshChanged();
}

} // namespace State
} // namespace Lattice
