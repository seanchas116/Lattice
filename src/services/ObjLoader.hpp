#pragma once
#include "../support/Shorthands.hpp"
#include <boost/filesystem/path.hpp>
#include <vector>

namespace Lattice {
namespace Document {
class MeshObject;
class Document;
} // namespace Document

namespace Services {

class ObjLoader final {
  public:
    static std::vector<SP<Document::MeshObject>> load(const SP<Document::Document> &document, const boost::filesystem::path &filePath);
};

} // namespace Services
} // namespace Lattice
