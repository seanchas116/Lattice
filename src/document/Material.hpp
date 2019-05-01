#pragma once
#include <QImage>
#include <glm/glm.hpp>
#include "../draw/Material.hpp"

namespace Lattice {
namespace Document {

class Material {
public:
    Material();

    // TODO: support undo/redo for Material changes

    glm::dvec3 baseColor() const { return _baseColor; }
    void setBaseColor(const glm::dvec3 &baseColor) { _baseColor = baseColor; }

    // TODO: image should be storead as a reference to an item of per-document image list
    QImage baseColorImage() const { return _baseColorImage; }
    void setBaseColorImage(const QImage &baseColorImage) { _baseColorImage = baseColorImage; }

    float metallic() const { return _metallic; }
    void setMetallic(float metallic) { _metallic = metallic; }

    QImage metallicImage() const { return _metallicImage; }
    void setMetallicImage(const QImage &metallicImage) { _metallicImage = metallicImage; }

    float roughness() const { return _roughness; }
    void setRoughness(float roughness) { _roughness = roughness; }

    QImage roughnessImage() const { return _roughnessImage; }
    void setRoughnessImage(const QImage &roughnessImage) { _roughnessImage = roughnessImage; }

    Draw::Material toDrawMaterial() const;

    bool operator==(const Material& other) const;
    bool operator!=(const Material& other) const { return !operator==(other); }

private:
    glm::dvec3 _baseColor {1, 0, 0};
    QImage _baseColorImage;

    float _metallic {0};
    QImage _metallicImage;

    float _roughness {0.5};
    QImage _roughnessImage;
};

} // namespace Document
} // namespace Lattice
