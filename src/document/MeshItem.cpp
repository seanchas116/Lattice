#include "MeshItem.hpp"
#include "../mesh/Mesh.hpp"
#include "../support/Debug.hpp"
#include <nlohmann/json.hpp>
#include <QTimer>

using namespace glm;

namespace Lattice {
namespace Document {

MeshItem::MeshItem() : _mesh(makeShared<Mesh::Mesh>()) {
    connect(_mesh.get(), &Mesh::Mesh::changed, this, &MeshItem::handleMeshChange);
    connect(this, &MeshItem::meshChangedInLastTick, this, &Object::changed);
    _mesh->setChangeHandler([this](const auto& change) {
        addChange(change);
    });
}

SP<Object> MeshItem::clone() const {
    auto cloned = makeShared<MeshItem>();
    // FIXME: item name is not copied
    cloned->_mesh = _mesh->clone();
    return cloned;
}

void MeshItem::toJSON(nlohmann::json &json) const {
    Object::toJSON(json);
    throw std::runtime_error("TODO");
    // TODO
    // _shape->toJSON(json["shape"]);
}

void MeshItem::fromJSON(const nlohmann::json &json) {
    Object::fromJSON(json);
    throw std::runtime_error("TODO");
    // TODO
    //_shape->fomJSON(json["shape"]);
}

void MeshItem::handleMeshChange() {
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
