#include "Image.hpp"

namespace Lattice {
namespace Document {

Image::Image(QImage image, std::string filePath) :
    _image(std::move(image)),
    _filePath(std::move(filePath)) {
}

Opt<SP<Image>> Image::open(const std::string &filePath) {
    QImage image(QString::fromStdString(filePath));
    if (image.isNull()) {
        return {};
    }
    return makeShared<Image>(image, filePath);
}

} // namespace Document
} // namespace Lattice
