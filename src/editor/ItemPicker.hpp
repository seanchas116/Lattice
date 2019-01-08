#pragma once
#include <QObject>
#include "../support/Pointer.hpp"
#include "../support/Ray.hpp"
#include "../support/Box.hpp"

namespace Lattice {
namespace Document {
class Item;
class MeshItem;
class Mesh;
}

namespace Editor {

class ItemPicker : public QObject {
    Q_OBJECT
public:
    ItemPicker();
    void update(const SP<Document::Item>& rootItem);

    std::optional<std::pair<SP<Document::Item>, float>> pick(const Ray<float>& worldMouseRay) const;

private:
    void addItemRecursive(const SP<Document::Item>& item);

    struct Entry {
        SP<Document::MeshItem> item;
        Box<float> box;
    };

    std::vector<Entry> _entries;
};

}
} // namespace Lattice
