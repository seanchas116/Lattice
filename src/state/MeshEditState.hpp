#pragma once
#include "../document/MeshObject.hpp"
#include "../mesh/Handle.hpp"
#include <QObject>

namespace Lattice {

namespace Mesh {
class Mesh;
}

namespace State {

class MeshEditState : public QObject {
    Q_OBJECT
public:
    MeshEditState(const SP<Document::MeshObject>& targetObject);

    auto& targetObject() const { return _targetObject; }

    auto& mesh() const { return _mesh; }
    void setMesh(Mesh::Mesh mesh);

    auto& selectedVertices() const { return _selectedVertices; }

    void notifyMeshChange();
    void commitMeshChange(const QString& changeTitle);

    void deleteVertices();
    void deleteEdges();
    void deleteFaces();
    void selectAll();
    void deselectAll();

signals:
    void meshChanged(const Mesh::Mesh& mesh);
    void selectedVerticesChanged(const std::vector<Mesh::VertexHandle>& vertices);
    void targetObjectChanged(const Opt<SP<Document::MeshObject>>& target);

private:
    SP<Document::MeshObject> _targetObject;
    SP<Mesh::Mesh> _mesh;
    std::vector<Mesh::VertexHandle> _selectedVertices;
};

} // namespace State
} // namespace Lattice

