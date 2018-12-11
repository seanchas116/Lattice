#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_precision.hpp>

namespace Lattice {

template <typename T>
class PixelData final {
public:
    PixelData(glm::ivec2 size) : _size(size), _data(size_t(size.x * size.y)) {}
    glm::ivec2 size() const { return _size; }
    const std::vector<T>& data() const { return _data; }
    std::vector<T>& data() { return _data; }

private:
    glm::ivec2 _size;
    std::vector<T> _data;
};

} // namespace Lattice
