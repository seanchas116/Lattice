#pragma once
#include "../document/MeshObject.hpp"
#include "../mesh/Handle.hpp"
#include <QObject>

namespace Lattice {

namespace State {

class MeshEditState : public QObject {
    Q_OBJECT
public:
    MeshEditState(const SP<Document::MeshObject>& object);

    auto& object() const { return _object; }

    auto& mesh() const { return _mesh; }
    void setMesh(Mesh::Mesh mesh);

    void notifyMeshChanged();
    void commitMeshChanged(const QString& changeTitle);

    void deleteVertices();
    void deleteEdges();
    void deleteFaces();

    void selectAll();
    void deselectAll();
    void invertSelection();
    void loopSelect(Mesh::EdgeHandle edge);

signals:
    void meshChanged(const Mesh::Mesh& mesh);

private:
    SP<Document::MeshObject> _object;
    SP<Mesh::Mesh> _mesh;
};

} // namespace State
} // namespace Lattice

