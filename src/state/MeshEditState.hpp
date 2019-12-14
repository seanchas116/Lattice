#pragma once
#include "../document/MeshObject.hpp"
#include <QObject>
#include <meshlib/Handle.hpp>

namespace Lattice {

namespace State {

class MeshEditState : public QObject {
    Q_OBJECT
  public:
    MeshEditState(const SP<Document::MeshObject> &object);

    auto &object() const { return _object; }

    auto &mesh() const { return _mesh; }
    void setMesh(meshlib::Mesh mesh);

    void notifyMeshChanged();
    void commitMeshChanged(const QString &changeTitle);

    void deleteVertices();
    void deleteEdges();
    void deleteFaces();

    void selectAll();
    void deselectAll();
    void invertSelection();
    void selectLoop(meshlib::EdgeHandle edge);
    void selectBelt(meshlib::EdgeHandle edge);
    void selectConnected(const std::vector<meshlib::VertexHandle> &vertices);

    void flipFaces();

  signals:
    void meshChanged(const meshlib::Mesh &mesh);

  private:
    SP<Document::MeshObject> _object;
    SP<meshlib::Mesh> _mesh;
};

} // namespace State
} // namespace Lattice
