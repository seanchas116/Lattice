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

class MeshVertex {
public:
    glm::vec3 position() const { return _position; }
    void setPosition(glm::vec3 position) { _position = position; }

    std::vector<SP<MeshEdge>> edges() const;
    std::vector<SP<MeshFace>> faces() const;

private:
    friend class MeshEdge;
    friend class MeshFace;
    glm::vec3 _position;
    std::unordered_set<MeshEdge*> _edges;
    std::unordered_set<MeshFace*> _faces;
};

class MeshEdge : public std::enable_shared_from_this<MeshEdge> {
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

class MeshFace : public std::enable_shared_from_this<MeshFace> {
public:
    MeshFace(const std::vector<SP<MeshVertex>>& vertices, const std::vector<SP<MeshEdge>>& edges);
    ~MeshFace();

    auto& vertices() const { return _vertices; }
    auto& edges() const { return _edges; }

    glm::vec3 normal() const;

private:
    WP<Mesh> _mesh;
    std::vector<SP<MeshVertex>> _vertices;
    std::vector<SP<MeshEdge>> _edges;
};

class MeshMaterial {
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

private:
    glm::vec3 _baseColor;
    QImage _baseColorImage;

    float _metallic;
    QImage _metallicImage;

    float _roughness;
    QImage _roughnessImage;
};

class Mesh {
public:
    Mesh();

    SP<MeshVertex> addVertex(glm::vec3 position);
    SP<MeshEdge> addEdge(const std::array<SP<MeshVertex>, 2>& vertices);
    SP<MeshFace> addFace(const std::vector<SP<MeshVertex>>& vertices);

    const auto& vertices() const { return _vertices; }
    const auto& edges() const { return _edges; }
    const auto& faces() const { return _faces; }

    SP<Mesh> clone() const;

private:

    std::unordered_set<SP<MeshVertex>> _vertices;
    std::unordered_map<std::array<SP<MeshVertex>, 2>, SP<MeshEdge>> _edges;
    std::unordered_map<std::vector<SP<MeshVertex>>, SP<MeshFace>> _faces;
};

} // namespace Lattice