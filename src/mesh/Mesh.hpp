#pragma once
#include "../support/Hash.hpp"
#include "../support/Shorthands.hpp"
#include "../support/Box.hpp"
#include "../support/SortedArray.hpp"
#include "../support/Change.hpp"
#include "../support/Ray.hpp"
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
    glm::dvec3 position() const { return _position; }

    auto& edges() const { return _edges; }
    auto& faces() const { return _faces; }
    auto& uvPoints() const { return _uvPoints; }

    SP<UVPoint> firstUVPoint() const;

private:
    friend class Mesh;
    glm::dvec3 _position {0};
    std::unordered_set<Edge*> _edges;
    std::unordered_set<Face*> _faces;
    std::unordered_set<UVPoint*> _uvPoints;
};

class Edge final : public EnableSharedFromThis<Edge> {
    Q_DISABLE_COPY(Edge)
public:
    Edge(const SortedArray<SP<Vertex>, 2>& vertices) : _vertices(vertices) {}

    bool isSmooth() const { return _isSmooth; }

    auto& vertices() const { return _vertices; }
    auto ray() const {
        auto p0 = _vertices[0]->position();
        auto p1 = _vertices[1]->position();
        return Ray<float>(p0, p1 - p0);
    }

    auto& faces() const { return _faces; }

private:
    friend class Mesh;
    bool _isSmooth = true;
    SortedArray<SP<Vertex>, 2> _vertices;
    std::unordered_set<Face*> _faces;
};

class UVPoint final : public EnableSharedFromThis<UVPoint> {
    Q_DISABLE_COPY(UVPoint)
public:
    UVPoint(const SP<Vertex>& vertex) : _vertex(vertex) {}

    glm::dvec2 position() const { return _position; }

    auto& vertex() const { return _vertex; }
    auto& faces() const { return _faces; }

private:
    friend class Mesh;
    glm::dvec2 _position {0};
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
        _vertexNormals(vertices.size()),
        _material(material)
    {}

    auto& vertices() const { return _vertices; }
    auto& edges() const { return _edges; }
    auto& uvPoints() const { return _uvPoints; }
    auto& vertexNormals() const { return _vertexNormals; }

    auto normal() const { return _normal; }

    auto& material() const { return _material; }
    void setMaterial(const SP<Material>& material);

private:
    void updateNormal();
    void updateVertexNormals();

    friend class Mesh;
    std::vector<SP<Vertex>> _vertices;
    std::vector<SP<Edge>> _edges;
    std::vector<SP<UVPoint>> _uvPoints;
    std::vector<glm::dvec3> _vertexNormals;
    glm::dvec3 _normal {0};
    SP<Material> _material;
};

class Material final {
    Q_DISABLE_COPY(Material)
public:
    Material() {}

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

    // returning container of raw pointers for performance
    auto& faces() const { return _faces; }

private:
    friend class Mesh;
    friend class Face;

    glm::dvec3 _baseColor {1, 0, 0};
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

    SP<Vertex> addVertex(glm::dvec3 position);
    SP<Edge> addEdge(const SortedArray<SP<Vertex>, 2>& vertices);

    SP<UVPoint> addUVPoint(const SP<Vertex>& vertex, glm::dvec2 position);

    SP<Face> addFace(const std::vector<SP<UVPoint>>& uvPoints, const SP<Material>& material);
    SP<Material> addMaterial();

    void setPosition(const std::unordered_map<SP<Vertex>, glm::dvec3>& positions);
    void setPosition(const std::unordered_map<SP<UVPoint>, glm::dvec2>& positions);
    void setSmooth(const std::unordered_map<SP<Edge>, bool>& values);

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

    SP<Face> flipFace(const SP<Face>& face);
    SP<Vertex> cutEdge(const SP<Edge>& edge, float t);

    SP<Mesh> clone() const;

    Box<float> boundingBox() const;

    void updateNormals();

signals:
    void vertexAdded(const SP<Vertex>& vertex);
    void vertexChanged(const SP<Vertex>& vertex);
    void vertexRemoved(const SP<Vertex>& vertex);

    void uvPointAdded(const SP<UVPoint>& uvPoint);
    void uvPointChanged(const SP<UVPoint>& uvPoint);
    void uvPointRemoved(const SP<UVPoint>& uvPoint);

    void edgeAdded(const SP<Edge>& edge);
    void edgeChanged(const SP<Edge>& edge);
    void edgeRemoved(const SP<Edge>& edge);

    void faceAdded(const SP<Face>& face);
    void faceChanged(const SP<Face>& face);
    void faceRemoved(const SP<Face>& face);

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
