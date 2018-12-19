#include "MeshRenderer.hpp"
#include "../document/MeshShape.hpp"
#include "../gl/VAO.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include <unordered_map>

using namespace glm;

namespace Lattice {

MeshRenderer::MeshRenderer() {
    _faceVAO = std::make_shared<VAO>();
    _edgeVAO = std::make_shared<LineVAO>();
}

void MeshRenderer::update(const SP<MeshShape> &shape) {
    {
        // Edge VAO
        std::unordered_map<SP<Vertex>, int> indices;
        std::vector<VertexBuffer::Vertex> vertices;
        std::vector<LineVAO::Line> lines;
        for (auto& vertex : shape->vertices()) {
            indices[vertex] = vertices.size();
            VertexBuffer::Vertex vertexData = {
                    vertex->position(),
                    vec2(0), // TODO
                    vec3(0) // TODO
            };
            vertices.push_back(vertexData);
        }
        for (auto& edge : shape->edges()) {
            uint32_t i0 = indices[edge->inVertex()];
            uint32_t i1 = indices[edge->outVertex()];
            lines.push_back({i0, i1});
        }
        _edgeVAO->vertexBuffer()->setVertices(vertices);
        _edgeVAO->setLines(lines);
    }

    {
        // Face VAO
        std::vector<VertexBuffer::Vertex> vertices;
        std::vector<VAO::Triangle> triangles;

        for (auto& face : shape->faces()) {
            uint32_t offset = vertices.size();
            auto normal = face->normal();
            for (auto& vertex : face->vertices()) {
                vertices.push_back({vertex->position(), {}, normal});
            }
            for (uint32_t i = 2; i < uint32_t(face->vertices().size()); ++i) {
                triangles.push_back({offset, offset + i - 1, offset + i});
            }
        }

        _faceVAO->vertexBuffer()->setVertices(vertices);
        _faceVAO->setTriangles(triangles);
    }
}

}