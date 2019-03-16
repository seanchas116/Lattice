#pragma once
#include <vector>
#include "../support/Shorthands.hpp"

class QString;

namespace Lattice {
namespace Document {
class MeshItem;
}

namespace Services {

class ObjLoader final {
public:
    static std::vector<SP<Document::MeshItem>> load(const QString &filePath);
};

}
} // namespace Lattice
