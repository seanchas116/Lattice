#include "MeshObject.hpp"
#include "../mesh/Mesh.hpp"
#include "../support/Debug.hpp"
#include <nlohmann/json.hpp>
#include <QTimer>

using namespace glm;

namespace Lattice {
namespace Document {

MeshObject::MeshObject() : _mesh(makeShared<OldMesh::Mesh>()) {
    connect(_mesh.get(), &OldMesh::Mesh::changed, this, &MeshObject::handleMeshChange);
    _mesh->setChangeHandler([this](const auto& change) {
        addChange(change);
    });
}

SP<Object> MeshObject::clone() const {
    auto cloned = makeShared<MeshObject>();
    // FIXME: object name is not copied
    cloned->_mesh = _mesh->clone();
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

void MeshObject::handleMeshChange() {
    if (!_meshChangedInTick) {
        _meshChangedInTick = true;
        QTimer::singleShot(0, this, [this] {
            _meshChangedInTick = false;
            emit meshChangedInLastTick();
        });
    }
}

}
} // namespace Lattice
