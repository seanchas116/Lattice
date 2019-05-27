#pragma once
#include <QObject>
#include "../support/Shorthands.hpp"

namespace Lattice {
namespace Document {

class Image;

class ImageManager : public QObject {
    Q_OBJECT
public:
    ImageManager();

    // TODO: support undo/redo for ImageManager changes

    auto& images() const { return _images; }

    Opt<SP<Image>> openImage(const std::string& filePath);

signals:
    void imagesChanged(const std::vector<SP<Image>>& images);

private:
    std::vector<SP<Image>> _images;
};

} // namespace Document
} // namespace Lattice
