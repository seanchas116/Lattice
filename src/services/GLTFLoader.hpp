#pragma once
#include "../support/Shorthands.hpp"
#include <vector>

class QString;

namespace Lattice {
namespace Document {
class MeshObject;
class Document;
} // namespace Document

namespace Services {

class GLTFLoader final {
  public:
    static std::vector<SP<Document::MeshObject>> load(const SP<Document::Document> &document, const QString &filePath);
};

} // namespace Services
} // namespace Lattice
