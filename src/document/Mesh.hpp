#pragma once
#include "../support/Hash.hpp"
#include "../support/Pointer.hpp"
#include "../support/Box.hpp"
#include "../support/SortedArray.hpp"
#include "Change.hpp"
#include <QObject>
#include <QImage>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <functional>

namespace Lattice::Document {

class MeshVertex;
class MeshEdge;
class MeshUVPoint;
class MeshFace;
class Mesh;
class MeshMaterial;

class MeshVertex final : public EnableSharedFromThis<MeshVertex> {
public:
    glm::vec3 position() const { return _position; }
    void setPosition(glm::vec3 position) { _position = position; }

    std::vector<SP<MeshEdge>> edges() const;
    std::vector<SP<MeshFace>> faces() const;
    auto& uvPoints() const { return _uvPoints; }

    glm::vec3 normal() const;

private:
    friend class Mesh;
    glm::vec3 _position;
    std::unordered_set<MeshEdge*> _edges;
    std::unordered_set<MeshFace*> _faces;
    std::unordered_set<SP<MeshUVPoint>> _uvPoints;
};

class MeshEdge final : public EnableSharedFromThis<MeshEdge> {
public:
    MeshEdge(const std::array<SP<MeshVertex>, 2>& vertices) : _vertices(vertices) {}

    auto& vertices() const { return _vertices; }
    std::vector<SP<MeshFace>> faces() const;

private:
    friend class Mesh;
    std::array<SP<MeshVertex>, 2> _vertices;
    std::unordered_set<MeshFace*> _faces;
};

class MeshUVPoint final : public EnableSharedFromThis<MeshUVPoint> {
public:
    glm::vec2 position() const { return _position; }
    void setPosition(glm::vec2 position) { _position = position; }

    SP<MeshVertex> vertex() const;
    std::vector<SP<MeshFace>> faces() const;

private:
    friend class Mesh;
    glm::vec2 _position;
    MeshVertex* _vertex;
    std::unordered_set<MeshFace*> _faces;
};

class MeshFace final : public EnableSharedFromThis<MeshFace> {
public:
    MeshFace(const std::vector<SP<MeshVertex>>& vertices, const std::vector<SP<MeshEdge>>& edges,
             const std::vector<SP<MeshUVPoint>>& uvPoints,
             const SP<MeshMaterial>& material) :
        _vertices(vertices),
        _edges(edges),
        _uvPoints(uvPoints),
        _material(material)
    {}

    auto& vertices() const { return _vertices; }
    auto& edges() const { return _edges; }
    auto& uvPoints() const { return _uvPoints; }

    glm::vec3 normal() const;

    auto& material() const { return _material; }
    void setMaterial(const SP<MeshMaterial>& material);

private:
    friend class Mesh;
    std::vector<SP<MeshVertex>> _vertices;
    std::vector<SP<MeshEdge>> _edges;
    std::vector<SP<MeshUVPoint>> _uvPoints;
    SP<MeshMaterial> _material;
};

class MeshMaterial final {
public:
    glm::vec3 baseColor() const { return _baseColor; }
    void setBaseColor(const glm::vec3 &baseColor) { _baseColor = baseColor; }

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

    std::vector<SP<MeshFace> > faces() const;

private:
    friend class Mesh;
    friend class MeshFace;

    glm::vec3 _baseColor {1, 0, 0};
    QImage _baseColorImage;

    float _metallic {0};
    QImage _metallicImage;

    float _roughness {0.5};
    QImage _roughnessImage;

    std::unordered_set<MeshFace*> _faces;
};

class Mesh final : public EnableSharedFromThis<Mesh> {
public:
    Mesh();

    void setChangeHandler(const std::function<void(const SP<Change>& change)> &changeHandler) { _changeHandler = changeHandler; }

    SP<MeshVertex> addVertex(glm::vec3 position);
    SP<MeshEdge> addEdge(const std::array<SP<MeshVertex>, 2>& vertices);

    SP<MeshUVPoint> addUVPoint(const SP<MeshVertex>& vertex, glm::vec2 position);

    SP<MeshFace> addFace(const std::vector<SP<MeshUVPoint>>& uvPoints, const SP<MeshMaterial>& material);
    SP<MeshMaterial> addMaterial();

    void removeFace(const SP<MeshFace>& face);
    void removeEdge(const SP<MeshEdge>& edge);
    void removeVertex(const SP<MeshVertex>& vertex);

    const auto& vertices() const { return _vertices; }
    const auto& edges() const { return _edges; }
    const auto& faces() const { return _faces; }
    const auto& materials() const { return _materials; }

    void addPlane(glm::dvec3 center, glm::dvec2 size, int normalAxis, const SP<MeshMaterial>& material);

    void addCube(glm::dvec3 minPos, glm::dvec3 maxPos, const SP<MeshMaterial>& material);

    enum class CircleFill {
        None,
        Ngon,
        TriangleFan,
    };
    void addCircle(glm::dvec3 center, double radius, int segmentCount, CircleFill fill, int normalAxis, const SP<MeshMaterial>& material);

    void addSphere(glm::dvec3 center, double radius, int segmentCount, int ringCount, int axis, const SP<MeshMaterial>& material);

    void addCone(glm::dvec3 center, double radius, double height, int segmentCount, int axis, const SP<MeshMaterial>& material);

    void merge(const SP<const Mesh> &other);

    SP<Mesh> clone() const;

    Box<float> boundingBox() const;

private:
    class AddVertexChange;

    void handleChange(const SP<Change>& change);

    std::unordered_set<SP<MeshVertex>> _vertices;
    std::unordered_map<SortedArray<SP<MeshVertex>, 2>, SP<MeshEdge>> _edges;
    std::unordered_map<std::set<SP<MeshVertex>>, SP<MeshFace>> _faces;
    std::vector<SP<MeshMaterial>> _materials;
    std::function<void(const SP<Change>& change)> _changeHandler;
};

} // namespace Lattice
