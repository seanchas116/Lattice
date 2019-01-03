#pragma once
#include <string>
#include <vector>
#include "../support/Pointer.hpp"

namespace Lattice::Document {
class MeshItem;
}

namespace Lattice::Services {

class ObjLoader final {
public:
    static std::vector<SP<Document::MeshItem>> load(const std::string& filePath);
};

} // namespace Lattice
