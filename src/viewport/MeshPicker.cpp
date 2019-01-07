#include "MeshPicker.hpp"

namespace Lattice {
namespace Viewport {

MeshPicker::MeshPicker(const SP<Document::Mesh> &mesh) : _mesh(mesh) {
}

std::pair<SP<Document::MeshFace>, double> MeshPicker::picKFace(const Ray<double> &ray) const {
    Q_UNUSED(ray);
    return {}; // TODO
}

std::pair<SP<Document::MeshVertex>, double> MeshPicker::pickVertex(const Ray<double> &ray, double distance) const {
    Q_UNUSED(ray);
    Q_UNUSED(distance);
    return {}; // TODO
}

std::pair<SP<Document::MeshEdge>, double> MeshPicker::pickEdge(const Ray<double> &ray, double distance) const {
    Q_UNUSED(ray);
    Q_UNUSED(distance);
    return {}; // TODO
}

} // namespace Viewport
} // namespace Lattice
