#include "GLTFLoader.hpp"
#include "../document/Document.hpp"
#include "../document/ImageManager.hpp"
#include "../document/MeshObject.hpp"
#include "../support/Debug.hpp"
#include <meshlib/Mesh.hpp>
#include <tinygltf/tiny_gltf.h>

using namespace glm;

namespace Lattice {
namespace Services {

std::vector<SP<Document::MeshObject>> GLTFLoader::load(const SP<Document::Document> &document, const QString &filePathString) {
    Q_UNUSED(document);
    Q_UNUSED(filePathString);
    return {};
}

} // namespace Services
} // namespace Lattice
