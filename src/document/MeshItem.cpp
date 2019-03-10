#include "MeshItem.hpp"
#include "../mesh/Mesh.hpp"
#include "../support/Debug.hpp"
#include <nlohmann/json.hpp>

using namespace glm;

namespace Lattice {
namespace Document {

MeshItem::MeshItem() : _mesh(makeShared<Mesh::Mesh>()) {
    connect(_mesh.get(), &Mesh::Mesh::changed, this, &Item::changed);
    _mesh->setChangeHandler([this](const auto& change) {
        addChange(change);
    });
}

SP<Item> MeshItem::clone() const {
    auto cloned = makeShared<MeshItem>();
    // FIXME: item name is not copied
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

}
} // namespace Lattice
