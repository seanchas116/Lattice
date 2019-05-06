#pragma once
#include "../document/MeshObject.hpp"
#include <QObject>

namespace Lattice {

namespace Mesh {
class Mesh;
}

namespace State {

class MeshEditState : public QObject {
    Q_OBJECT
public:
    MeshEditState(const SP<Document::MeshObject>& object);

    auto& object() const { return _object; }

    auto& mesh() const { return _mesh; }
    void setMesh(Mesh::Mesh mesh);

    void notifyMeshChange();
    void commitMeshChange(const QString& changeTitle);

    void deleteVertices();
    void deleteEdges();
    void deleteFaces();
    void selectAll();
    void deselectAll();

signals:
    void meshChanged(const Mesh::Mesh& mesh);
    void targetObjectChanged(const Opt<SP<Document::MeshObject>>& target);

private:
    SP<Document::MeshObject> _object;
    SP<Mesh::Mesh> _mesh;
};

} // namespace State
} // namespace Lattice

