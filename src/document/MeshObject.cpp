#include "MeshObject.hpp"
#include "../oldmesh/Mesh.hpp"
#include "../support/Debug.hpp"
#include <nlohmann/json.hpp>
#include <QTimer>

using namespace glm;

namespace Lattice {
namespace Document {

MeshObject::MeshObject() : _oldMesh(makeShared<OldMesh::Mesh>()) {
    connect(_oldMesh.get(), &OldMesh::Mesh::changed, this, &MeshObject::handleOldMeshChange);
    _oldMesh->setChangeHandler([this](const auto& change) {
        addChange(change);
    });
}

void MeshObject::setMesh(Mesh::Mesh &&mesh) {
    _mesh = std::move(mesh);
    emit meshChanged(_mesh);
}

SP<Object> MeshObject::clone() const {
    auto cloned = makeShared<MeshObject>();
    // FIXME: object name is not copied
    cloned->_oldMesh = _oldMesh->clone();
    return cloned;
}

void MeshObject::toJSON(nlohmann::json &json) const {
    Object::toJSON(json);
    throw std::runtime_error("TODO");
    // TODO
    // _shape->toJSON(json["shape"]);
}

void MeshObject::fromJSON(const nlohmann::json &json) {
    Object::fromJSON(json);
    throw std::runtime_error("TODO");
    // TODO
    //_shape->fomJSON(json["shape"]);
}

void MeshObject::handleOldMeshChange() {
    if (!_oldMeshChangedInTick) {
        _oldMeshChangedInTick = true;
        QTimer::singleShot(0, this, [this] {
            _oldMeshChangedInTick = false;
            emit oldMeshChangedInLastTick();
        });
    }
}

}
} // namespace Lattice
