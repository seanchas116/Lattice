#pragma once
#include <vector>
#include "../support/Shorthands.hpp"

class QString;

namespace Lattice::Document {
class MeshItem;
}

namespace Lattice::Services {

class ObjLoader final {
public:
    static std::vector<SP<Document::MeshItem>> load(const QString &filePath);
};

} // namespace Lattice
