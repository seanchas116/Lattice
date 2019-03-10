#pragma once
#include "../support/Hash.hpp"
#include "../support/Shorthands.hpp"
#include "../support/Box.hpp"
#include "../support/SortedArray.hpp"
#include "../support/Change.hpp"
#include <QObject>
#include <QImage>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <functional>

namespace Lattice::Mesh {

class Vertex;
class Edge;
class UVPoint;
class Face;
class Mesh;
class Material;

class Vertex final : public EnableSharedFromThis<Vertex> {
    Q_DISABLE_COPY(Vertex)
public:
    Vertex() {}
    glm::vec3 position() const { return _position; }

    auto& edges() const { return _edges; }
    auto& faces() const { return _faces; }
    auto& uvPoints() const { return _uvPoints; }

    SP<UVPoint> firstUVPoint() const;

    glm::vec3 normal() const;

private:
    friend class Mesh;
    glm::vec3 _position {0};
    std::unordered_set<Edge*> _edges;
    std::unordered_set<Face*> _faces;
    std::unordered_set<UVPoint*> _uvPoints;
};

class Edge final : public EnableSharedFromThis<Edge> {
    Q_DISABLE_COPY(Edge)
public:
    Edge(const std::array<SP<Vertex>, 2>& vertices) : _vertices(vertices) {}

    auto& vertices() const { return _vertices; }
    std::vector<SP<Face>> faces() const;

private:
    friend class Mesh;
    std::array<SP<Vertex>, 2> _vertices;
    std::unordered_set<Face*> _faces;
};

class UVPoint final : public EnableSharedFromThis<UVPoint> {
    Q_DISABLE_COPY(UVPoint)
public:
    UVPoint(const SP<Vertex>& vertex) : _vertex(vertex) {}

    glm::vec2 position() const { return _position; }

    auto& vertex() const { return _vertex; }
    std::vector<SP<Face>> faces() const;

private:
    friend class Mesh;
    glm::vec2 _position {0};
    SP<Vertex> _vertex;
    std::unordered_set<Face*> _faces;
};

class Face final : public EnableSharedFromThis<Face> {
    Q_DISABLE_COPY(Face)
public:
    Face(const std::vector<SP<Vertex>>& vertices, const std::vector<SP<Edge>>& edges,
             const std::vector<SP<UVPoint>>& uvPoints,
             const SP<Material>& material) :
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
    void setMaterial(const SP<Material>& material);

private:
    friend class Mesh;
    std::vector<SP<Vertex>> _vertices;
    std::vector<SP<Edge>> _edges;
    std::vector<SP<UVPoint>> _uvPoints;
    SP<Material> _material;
};

class Material final {
    Q_DISABLE_COPY(Material)
public:
    Material() {}

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

    // returning container of raw pointers for performance
    auto& faces() const { return _faces; }

private:
    friend class Mesh;
    friend class Face;

    glm::vec3 _baseColor {1, 0, 0};
    QImage _baseColorImage;

    float _metallic {0};
    QImage _metallicImage;

    float _roughness {0.5};
    QImage _roughnessImage;

    std::unordered_set<Face*> _faces;
};

class Mesh final : public QObject, public EnableSharedFromThis<Mesh> {
    Q_OBJECT
public:
    Mesh();

    void setChangeHandler(const Fn<void(const SP<Change>& change)> &changeHandler) { _changeHandler = changeHandler; }

    SP<Vertex> addVertex(glm::vec3 position);
    SP<Edge> addEdge(const std::array<SP<Vertex>, 2>& vertices);

    SP<UVPoint> addUVPoint(const SP<Vertex>& vertex, glm::vec2 position);

    SP<Face> addFace(const std::vector<SP<UVPoint>>& uvPoints, const SP<Material>& material);
    SP<Material> addMaterial();

    void setPositions(const std::unordered_map<SP<Vertex>, glm::vec3>& positions);
    void setPositions(const std::unordered_map<SP<UVPoint>, glm::vec2>& positions);

    void removeFace(const SP<Face>& face);
    void removeEdge(const SP<Edge>& edge);
    void removeVertex(const SP<Vertex>& vertex);

    const auto& vertices() const { return _vertices; }
    const auto& edges() const { return _edges; }
    const auto& faces() const { return _faces; }
    const auto& materials() const { return _materials; }

    void addPlane(glm::dvec3 center, glm::dvec2 size, int normalAxis, const SP<Material>& material);

    void addCube(glm::dvec3 minPos, glm::dvec3 maxPos, const SP<Material>& material);

    enum class CircleFill {
        None,
        Ngon,
        TriangleFan,
    };
    void addCircle(glm::dvec3 center, double radius, int segmentCount, CircleFill fill, int normalAxis, const SP<Material>& material);

    void addSphere(glm::dvec3 center, double radius, int segmentCount, int ringCount, int axis, const SP<Material>& material);

    void addCone(glm::dvec3 center, double radius, double height, int segmentCount, int axis, const SP<Material>& material);

    void addCylinder(glm::dvec3 center, double radius, double height, int segmentCount, int axis, const SP<Material>& material);

    void merge(const SP<const Mesh> &other);

    SP<Mesh> clone() const;

    Box<float> boundingBox() const;

signals:
    void vertexAdded(const SP<Vertex>& vertex);
    void vertexRemoved(const SP<Vertex>& vertex);

    void uvPointAdded(const SP<UVPoint>& uvPoint);
    void uvPointRemoved(const SP<UVPoint>& uvPoint);

    void edgeAdded(const SP<Edge>& edge);
    void edgeRemoved(const SP<Edge>& edge);

    void faceAdded(const SP<Face>& face);
    void faceRemoved(const SP<Face>& face);

    void verticesChanged(const std::vector<SP<Vertex>>& vertices);
    void uvPointsChanged(const std::vector<SP<UVPoint>>& uvPoint);

    void topologyChanged();
    void changed();

private:
    class AddVertexChange;
    class RemoveVertexChange;
    class AddUVPointChange;
    class RemoveUVPointChange;
    class AddEdgeChange;
    class RemoveEdgeChange;
    class AddFaceChange;
    class RemoveFaceChange;
    class SetVertexPositionChange;
    class SetUVPositionChange;

    void handleChange(const SP<Change>& change);

    std::unordered_set<SP<Vertex>> _vertices;
    std::unordered_map<SortedArray<SP<Vertex>, 2>, SP<Edge>> _edges;
    std::unordered_map<std::set<SP<Vertex>>, SP<Face>> _faces;
    std::vector<SP<Material>> _materials;
    Fn<void(const SP<Change>& change)> _changeHandler;
};

} // namespace Lattice
