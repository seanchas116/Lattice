#include "MeshItem.hpp"
#include "Mesh.hpp"
#include "../support/Debug.hpp"
#include <nlohmann/json.hpp>

using namespace glm;

namespace Lattice {

MeshItem::MeshItem() : _mesh(std::make_shared<Mesh>()) {
    connect(this, &MeshItem::meshChanged, this, &Item::changed);
}

SP<Item> MeshItem::clone() const {
    auto cloned = std::make_shared<MeshItem>();
    cloned->_mesh = _mesh->clone();
    return cloned;
}

void MeshItem::toJSON(nlohmann::json &json) const {
    Item::toJSON(json);
    throw std::runtime_error("TODO");
    // TODO
    // _shape->toJSON(json["shape"]);
}

void MeshItem::fromJSON(const nlohmann::json &json) {
    Item::fromJSON(json);
    throw std::runtime_error("TODO");
    // TODO
    //_shape->fromJSON(json["shape"]);
}

void MeshItem::setMesh(const SP<Mesh> &mesh) {
    emit meshChanged(mesh);
    _mesh = mesh;
}

} // namespace Lattice
