#pragma once
#include <vector>
#include "../support/Shorthands.hpp"

class QString;

namespace Lattice {
namespace Document {
class MeshObject;
class Document;
}

namespace Services {

class GLTFLoader final {
public:
    static std::vector<SP<Document::MeshObject>> load(const SP<Document::Document>& document, const QString &filePath);
};

}
} // namespace Lattice
