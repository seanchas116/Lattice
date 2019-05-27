#pragma once
#include <QObject>
#include <unordered_map>
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
    std::unordered_map<std::string, SP<Image>> _imageForPath;
};

} // namespace Document
} // namespace Lattice
