#pragma once
#include <QImage>
#include <glm/glm.hpp>
#include "../support/Shorthands.hpp"
#include "../draw/Material.hpp"
#include "Image.hpp"

namespace Lattice {
namespace Document {

class Material {
public:
    Material();

    // TODO: support undo/redo for Material changes

    glm::dvec3 baseColor() const { return _baseColor; }
    void setBaseColor(const glm::dvec3 &baseColor) { _baseColor = baseColor; }

    auto& baseColorImage() const { return _baseColorImage; }
    void setBaseColorImage(Opt<SP<Image>> baseColorImage) { _baseColorImage = std::move(baseColorImage); }

    float metallic() const { return _metallic; }
    void setMetallic(float metallic) { _metallic = metallic; }

    auto& metallicImage() const { return _metallicImage; }
    void setMetallicImage(Opt<SP<Image>> metallicImage) { _metallicImage = std::move(metallicImage); }

    float roughness() const { return _roughness; }
    void setRoughness(float roughness) { _roughness = roughness; }

    auto& roughnessImage() const { return _roughnessImage; }
    void setRoughnessImage(Opt<SP<Image>> roughnessImage) { _roughnessImage = std::move(roughnessImage); }

    Draw::Material toDrawMaterial() const;

    bool operator==(const Material& other) const;
    bool operator!=(const Material& other) const { return !operator==(other); }

private:
    glm::dvec3 _baseColor {1, 0, 0};
    Opt<SP<Image>> _baseColorImage;

    float _metallic {0};
    Opt<SP<Image>> _metallicImage;

    float _roughness {0.5};
    Opt<SP<Image>> _roughnessImage;
};

} // namespace Document
} // namespace Lattice
