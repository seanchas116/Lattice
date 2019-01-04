#pragma once
#include <QObject>
#include "../support/Pointer.hpp"
#include "../support/Ray.hpp"

namespace Lattice {
namespace Document {
class Item;
class MeshItem;
class Mesh;
}

namespace Viewport {

class ItemPicker : public QObject {
    Q_OBJECT
public:
    ItemPicker();
    void update(const SP<Document::Item>& rootItem);

    SP<Document::Item> pick(const Ray<float>& worldMouseRay) const;

private:
    void addItemRecursive(const SP<Document::Item>& item);
    static std::tuple<bool, float> intersectsRayMesh(const Ray<float>& ray, const SP<Document::Mesh>& mesh);

    std::vector<SP<Document::MeshItem>> _items;
};

}
} // namespace Lattice
