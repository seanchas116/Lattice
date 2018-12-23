#include "Mesh.hpp"
#include "../support/Mod.hpp"
#include <unordered_map>

using namespace glm;

namespace Lattice {

void Mesh::Vertex::removeEdge(Edge *edge) {
    for (auto it = _edges.begin(); it != _edges.end(); ++it) {
        if (*it == edge) {
            _edges.erase(it);
            return;
        }
    }
}

std::vector<SP<Mesh::Edge>> Mesh::Vertex::edges() const {
    std::vector<SP<Edge>> edges;
    for (auto&& edge : _edges) {
        edges.push_back(edge->shared_from_this());
    }
    return edges;
}

Mesh::Edge::Edge(const SP<Mesh::Vertex> &inVertex, const SP<Vertex> &outVertex) : _inVertex(inVertex), _outVertex(outVertex) {
    inVertex->_edges.push_back(this);
    outVertex->_edges.push_back(this);
}

Mesh::Edge::~Edge() {
    _inVertex->removeEdge(this);
    _outVertex->removeEdge(this);
}

std::vector<SP<Mesh::Face>> Mesh::Edge::faces() const {
    std::vector<SP<Face>> faces;
    for (auto&& face : _faces) {
        faces.push_back(face->shared_from_this());
    }
    return faces;
}

void Mesh::Edge::removeFace(Mesh::Face *face) {
    for (auto it = _faces.begin(); it != _faces.end(); ++it) {
        if (*it == face) {
            _faces.erase(it);
            return;
        }
    }
}

namespace {

std::vector<SP<Mesh::Vertex>> edgeLoopToVertices(const std::vector<SP<Mesh::Edge>>& edges) {
    if (edges.size() < 3) {
        throw std::runtime_error("edge count must >= 3");
    }

    std::unordered_set<SP<Mesh::Edge>> usedEdges;
    std::vector<SP<Mesh::Vertex>> vertices;

    vertices.push_back(edges[0]->inVertex());
    vertices.push_back(edges[0]->outVertex());
    usedEdges.insert(edges[0]);

    while (true) {
        auto& lastVertex = *(vertices.end() - 1);

        // find next edge
        SP<Mesh::Edge> nextEdge;
        SP<Mesh::Vertex> nextVertex;
        for (auto& edge : edges) {
            if (usedEdges.find(edge) != usedEdges.end()) {
                continue;
            }
            if (edge->inVertex() == lastVertex) {
                nextEdge = edge;
                nextVertex = edge->outVertex();
                break;
            }
            if (edge->outVertex() == lastVertex) {
                nextEdge = edge;
                nextVertex = edge->inVertex();
                break;
            }
        }
        if (!nextEdge) {
            throw std::runtime_error("cannot create loop");
        }

        if (vertices[0] == nextVertex) {
            if (vertices.size() != edges.size()) {
                throw std::runtime_error("found extra edges");
            }
            return vertices;
        }

        usedEdges.insert(nextEdge);
        vertices.push_back(nextVertex);
    }
}

}

Mesh::Face::Face(const std::vector<SP<Edge>> &edges) : _edges(edges)
{
    _vertices = edgeLoopToVertices(edges);

    for (auto&& edge : edges) {
        edge->_faces.push_back(this);
    }
}

Mesh::Face::~Face() {
    for (auto&& edge : _edges) {
        edge->removeFace(this);
    }
}

vec3 Mesh::Face::normal() const {
    if (_vertices.size() == 3) {
        return normalize(cross(_vertices[1]->position() - _vertices[0]->position(), _vertices[2]->position() - _vertices[0]->position()));
    }

    // find average vertex normal
    glm::vec3 normalSum(0);
    size_t count = _vertices.size();

    for (size_t i = 0; i < count; ++i) {
        auto prev = _vertices[mod(i - 1, count)]->position();
        auto curr = _vertices[i]->position();
        auto next = _vertices[mod(i + 1, count)]->position();
        auto normal = normalize(cross(next - curr, prev - curr));
        normalSum += normal;
    }

    return normalSum / float(count);
}

Mesh::~Mesh() {
}

SP<Mesh::Vertex> Mesh::addVertex(vec3 pos) {
    auto vertex = std::make_shared<Vertex>(pos);
    _vertices.insert(vertex);
    return vertex;
}

SP<Mesh::Edge> Mesh::addEdge(const SP<Vertex>& inVertex, const SP<Vertex>& outVertex) {
    auto edge = std::make_shared<Edge>(inVertex, outVertex);
    _edges.insert(edge);
    return edge;
}

SP<Mesh::Face> Mesh::addFace(const std::vector<SP<Edge> > &edges) {
    auto face = std::make_shared<Face>(edges);
    _faces.insert(face);
    return face;
}

void Mesh::removeVertex(const SP<Vertex>& vertex) {
    auto edges = vertex->edges();
    for (auto&& edge : edges) {
        removeEdge(edge);
    }
    _vertices.erase(vertex);
}

void Mesh::removeEdge(const SP<Edge> &edge) {
    auto faces = edge->faces();
    for (auto&& face : faces) {
        removeFace(face);
    }
    _edges.erase(edge);
}

void Mesh::removeFace(const SP<Face> &face) {
    _faces.erase(face);
}

std::shared_ptr<Mesh> Mesh::clone() const {
    auto cloned = std::make_shared<Mesh>();
    std::unordered_map<SP<Vertex>, SP<Vertex>> oldNewVertices;
    for (auto&& oldVertex : _vertices) {
        auto newVertex = cloned->addVertex(oldVertex->position());
        oldNewVertices[oldVertex] = newVertex;
    }
    for (auto&& oldEdge : _edges) {
        cloned->addEdge(oldNewVertices[oldEdge->inVertex()], oldNewVertices[oldEdge->outVertex()]);
    }
    return cloned;
}

} // namespace Lattice
