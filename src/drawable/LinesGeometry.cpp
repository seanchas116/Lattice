#include "LinesGeometry.hpp"
#include "../gl/VAO.hpp"
#include "../gl/IndexBuffer.hpp"
#include "../gl/VertexBuffer.hpp"

namespace Lattice {
namespace Drawable {

LinesGeometry::LinesGeometry() : _vbo(makeShared<GL::VertexBuffer<Point>>()),
                                 _ibo(makeShared<GL::IndexBuffer>()),
                                 _vao(makeShared<GL::VAO>(_vbo, _ibo)) {
}

void LinesGeometry::setPoints(const std::vector<Point> &points) {
    _vbo->setVertices(points);
}

void LinesGeometry::setLines(const std::vector<std::array<uint32_t, 2>> &lines) {
    _ibo->setLines(lines);
}

void LinesGeometry::setLineStrips(const std::vector<std::vector<uint32_t> > &lineStrips) {
    _ibo->setLineStrips(lineStrips);
}

} // namespace Drawable
} // namespace Lattice
