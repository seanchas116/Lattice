#pragma once
#include <QImage>
#include <string>

namespace Lattice {
namespace Document {

class Image {
public:
    Image(const std::string& filePath);

    auto& image() const { return _image; }
    auto& filePath() const { return _filePath; }

private:
    QImage _image;
    std::string _filePath;
};

} // namespace Document
} // namespace Lattice
