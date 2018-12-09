#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <unordered_set>
#include <memory>
#include <QtGlobal>
#include "../support/Pointer.hpp"

namespace Lattice {

class Edge;
class Face;

class Vertex final {
    Q_DISABLE_COPY(Vertex)
public:
    Vertex(glm::vec3 position) : _position(position) {}

    glm::vec3 position() const { return _position; }
    void setPosition(glm::vec3 position) { _position = position; }

    std::vector<SP<Edge>> edges() const;

private:
    friend class Edge;

    void removeEdge(Edge* edge);

    glm::vec3 _position;
    std::vector<Edge*> _edges;
};

class Edge final : public std::enable_shared_from_this<Edge> {
    Q_DISABLE_COPY(Edge)
public:
    Edge(const SP<Vertex>& inVertex, const SP<Vertex>& outVertex);
    ~Edge();

    SP<Vertex> inVertex() const { return _inVertex; }
    SP<Vertex> outVertex() const { return _outVertex; }

    std::vector<SP<Face>> faces() const;

private:
    friend class Face;

    void removeFace(Face* face);

    SP<Vertex> _inVertex;
    SP<Vertex> _outVertex;
    std::vector<Face*> _faces;
};

class Face final : public std::enable_shared_from_this<Face> {
    Q_DISABLE_COPY(Face)
public:
    Face(const std::vector<SP<Edge>>& edges);
    ~Face();

    const auto& edges() const { return _edges; }
    const auto& vertices() const { return _vertices; }
    glm::vec3 normal() const;

private:
    std::vector<SP<Edge>> _edges;
    std::vector<SP<Vertex>> _vertices;
};

class MeshShape final {
    Q_DISABLE_COPY(MeshShape)
public:
    MeshShape() = default;
    ~MeshShape();

    const auto& edges() const { return _edges; }
    const auto& vertices() const { return _vertices; }
    const auto& faces() const { return _faces; }

    SP<Vertex> addVertex(glm::vec3 pos);
    SP<Edge> addEdge(const SP<Vertex>& inVertex, const SP<Vertex>& outVertex);
    SP<Face> addFace(const std::vector<SP<Edge>>& edges);

    void removeVertex(const SP<Vertex>& vertex);
    void removeEdge(const SP<Edge>& edge);
    void removeFace(const SP<Face>& face);

    std::shared_ptr<MeshShape> clone() const;

private:
    std::unordered_set<SP<Edge>> _edges;
    std::unordered_set<SP<Vertex>> _vertices;
    std::unordered_set<SP<Face>> _faces;
};

} // namespace Shapecraft
