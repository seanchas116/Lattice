#pragma once
#include <vector>
#include <boost/filesystem/path.hpp>
#include "../support/Shorthands.hpp"

class QString;

namespace Lattice {
namespace Document {
class MeshObject;
class Document;
}

namespace Services {

class ObjLoader final {
public:
    static std::vector<SP<Document::MeshObject>> load(const SP<Document::Document>& document, const boost::filesystem::path &filePath);
};

}
} // namespace Lattice
