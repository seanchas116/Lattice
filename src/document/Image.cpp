#include "Image.hpp"

namespace Lattice {
namespace Document {

Image::Image(const std::string &filePath) :
    _image(QImage(QString::fromStdString(filePath))),
    _filePath(filePath) {
}

} // namespace Document
} // namespace Lattice
