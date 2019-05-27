#include "ImageManager.hpp"
#include "Image.hpp"

namespace Lattice {
namespace Document {

ImageManager::ImageManager() {
}

Opt<SP<Image>> ImageManager::openImage(const std::string &filePath) {
    if (_imageForPath.find(filePath) != _imageForPath.end()) {
        return _imageForPath.at(filePath);
    }

    // TODO: check if same image exists

    auto image = Image::open(filePath);
    if (!image) {
        return {};
    }

    _images.push_back(*image);
    _imageForPath.insert({filePath, *image});
    emit imagesChanged(_images);

    return image;
}

} // namespace Document
} // namespace Lattice
