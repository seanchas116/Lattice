#pragma once
#include <vector>
#include "../support/Shorthands.hpp"

class QString;

namespace Lattice {
namespace Document {
class MeshObject;
}

namespace Services {

class ObjLoader final {
public:
    static std::vector<SP<Document::MeshObject>> load(const QString &filePath);
};

}
} // namespace Lattice
