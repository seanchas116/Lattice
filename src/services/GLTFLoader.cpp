#include "GLTFLoader.hpp"
#include "../mesh/Mesh.hpp"
#include "../document/MeshObject.hpp"
#include "../document/Document.hpp"
#include "../document/ImageManager.hpp"
#include "../support/Debug.hpp"
#include <tinygltf/tiny_gltf.h>

using namespace glm;

namespace Lattice {
namespace Services {

std::vector<SP<Document::MeshObject>> GLTFLoader::load(const SP<Document::Document> &document, const std::string &filePathString) {
    Q_UNUSED(document);
    Q_UNUSED(filePathString);
    return {};
}

}
} // namespace Lattice
