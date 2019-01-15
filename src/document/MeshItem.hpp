#pragma once
#include "Item.hpp"
#include <glm/glm.hpp>

namespace Lattice::Document {

class Mesh;

class MeshItem final : public Item {
    Q_OBJECT
public:
    MeshItem();

    const SP<Mesh>& mesh() const { return _mesh; }
    void setMesh(const SP<Mesh> &mesh);
    void emitMeshChanged(); // TODO: better way to notify

    SP<Item> clone() const override;
    void toJSON(nlohmann::json& json) const override;
    void fromJSON(const nlohmann::json& json) override;

signals:
    void meshChanged(const SP<Mesh> &mesh);

private:
    SP<Mesh> _mesh;
};

} // namespace Lattice
