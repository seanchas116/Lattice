#pragma once
#include "../support/Hash.hpp"
#include "../support/Pointer.hpp"
#include <QImage>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace Lattice {

class MeshVertex;
class MeshEdge;
class MeshFace;
class Mesh;
class MeshMaterial;

class MeshVertex final {
public:
    glm::vec3 position() const { return _position; }
    void setPosition(glm::vec3 position) { _position = position; }
    glm::vec2 texCoord() { return _texCoord; }
    void setTexCoord(glm::vec2 texCoord) { _texCoord = texCoord; }

    std::vector<SP<MeshEdge>> edges() const;
    std::vector<SP<MeshFace>> faces() const;

    glm::vec3 normal() const;

private:
    friend class MeshEdge;
    friend class MeshFace;
    glm::vec3 _position;
    glm::vec2 _texCoord;
    std::unordered_set<MeshEdge*> _edges;
    std::unordered_set<MeshFace*> _faces;
};

class MeshEdge final : public std::enable_shared_from_this<MeshEdge> {
public:
    MeshEdge(const std::array<SP<MeshVertex>, 2>& vertices);
    ~MeshEdge();

    auto& vertices() const { return _vertices; }
    std::vector<SP<MeshFace>> faces() const;

private:
    friend class MeshFace;
    std::array<SP<MeshVertex>, 2> _vertices;
    std::unordered_set<MeshFace*> _faces;
};

class MeshFace final : public std::enable_shared_from_this<MeshFace> {
public:
    MeshFace(const std::vector<SP<MeshVertex>>& vertices, const std::vector<SP<MeshEdge>>& edges, const SP<MeshMaterial>& material);
    ~MeshFace();

    auto& vertices() const { return _vertices; }
    auto& edges() const { return _edges; }

    glm::vec3 normal() const;

    auto& material() const { return _material; }
    void setMaterial(const SP<MeshMaterial>& material);

private:
    std::vector<SP<MeshVertex>> _vertices;
    std::vector<SP<MeshEdge>> _edges;
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
    friend class MeshFace;

    glm::vec3 _baseColor {1, 0, 0};
    QImage _baseColorImage;

    float _metallic {0};
    QImage _metallicImage;

    float _roughness {0.5};
    QImage _roughnessImage;

    std::unordered_set<MeshFace*> _faces;
};

class Mesh final {
public:
    Mesh();

    SP<MeshVertex> addVertex(glm::vec3 position, glm::vec2 texCoord);
    SP<MeshEdge> addEdge(const std::array<SP<MeshVertex>, 2>& vertices);
    SP<MeshFace> addFace(const std::vector<SP<MeshVertex>>& vertices, const SP<MeshMaterial>& material);
    SP<MeshMaterial> addMaterial();

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

    void addSphere(glm::dvec3 center, double radius, int segmentCount, int ringCount, const SP<MeshMaterial>& material);

    SP<Mesh> clone() const;

private:

    std::unordered_set<SP<MeshVertex>> _vertices;
    std::unordered_map<std::array<SP<MeshVertex>, 2>, SP<MeshEdge>> _edges;
    std::unordered_map<std::vector<SP<MeshVertex>>, SP<MeshFace>> _faces;
    std::vector<SP<MeshMaterial>> _materials;
};

} // namespace Lattice
