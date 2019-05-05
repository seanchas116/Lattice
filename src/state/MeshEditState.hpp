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
    MeshEditState();

    auto& targetObject() const { return _targetObject; }
    void setTargetObject(Opt<SP<Document::MeshObject>> target);

    auto& mesh() const { return _mesh; }
    void setMesh(Mesh::Mesh mesh);

    void notifyMeshChange();
    void commitMeshChange(const QString& changeTitle);

signals:
    void meshChanged(const Mesh::Mesh& mesh);
    void targetObjectChanged(const Opt<SP<Document::MeshObject>>& target);

private:
    Opt<SP<Document::MeshObject>> _targetObject;
    SP<Mesh::Mesh> _mesh;
};

} // namespace State
} // namespace Lattice

