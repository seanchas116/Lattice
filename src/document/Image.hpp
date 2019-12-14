#pragma once
#include "../support/Shorthands.hpp"
#include <QImage>
#include <string>

namespace Lattice {
namespace Document {

class Image {
  public:
    Image(QImage image, std::string filePath);

    auto &image() const { return _image; }
    auto &filePath() const { return _filePath; }

    static Opt<SP<Image>> open(const std::string &filePath);

  private:
    QImage _image;
    std::string _filePath;
};

} // namespace Document
} // namespace Lattice
