#include "Material.hpp"

namespace Lattice {
namespace Document {

Material::Material() {
}

Draw::Material Material::toDrawMaterial() const {
    Draw::Material material;
    material.baseColor = _baseColor;
    material.baseColorImage = _baseColorImage ? (*_baseColorImage)->image() : QImage();
    material.metallic = _metallic;
    material.metallicImage = _metallicImage ? (*_metallicImage)->image() : QImage();
    material.roughness = _roughness;
    material.roughnessImage = _roughnessImage ? (*_roughnessImage)->image() : QImage();
    return material;
}

bool Material::operator==(const Material &other) const {
    return _baseColor == other._baseColor && _baseColorImage == other._baseColorImage && _metallic == other._metallic && _metallicImage == other._metallicImage && _roughness == other._roughness && _roughnessImage == other._roughnessImage;
}

} // namespace Document
} // namespace Lattice
