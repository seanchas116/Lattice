#include "Image.hpp"

namespace Lattice {
namespace Document {

Image::Image(std::string filePath) :
    _image(QImage(QString::fromStdString(filePath))),
    _filePath(std::move(filePath)) {
}

} // namespace Document
} // namespace Lattice
