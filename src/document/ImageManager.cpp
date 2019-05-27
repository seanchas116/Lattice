#include "ImageManager.hpp"
#include "Image.hpp"

namespace Lattice {
namespace Document {

ImageManager::ImageManager() {
}

SP<Image> ImageManager::openImage(const std::string &filePath) {
    // TODO: check if same image exists

    auto image = makeShared<Image>(filePath);
    _images.push_back(image);

    emit imagesChanged(_images);

    return image;
}

} // namespace Document
} // namespace Lattice
