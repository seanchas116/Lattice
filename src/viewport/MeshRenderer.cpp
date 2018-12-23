#include "MeshRenderer.hpp"
#include "Operations.hpp"
#include "../document/Mesh.hpp"
#include "../gl/VAO.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/PointVAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include <unordered_map>

using namespace glm;

namespace Lattice {

MeshRenderer::MeshRenderer() {
    _faceVAO = std::make_shared<VAO>();

    auto vbo = std::make_shared<VertexBuffer>();
    _edgeVAO = std::make_shared<LineVAO>(vbo);
    _vertexVAO = std::make_shared<PointVAO>(vbo);
}

void MeshRenderer::update(const SP<Mesh> &mesh) {
    {
        // Edge VAO
        std::unordered_map<SP<MeshVertex>, uint32_t> indices;
        std::vector<VertexBuffer::Vertex> vertices;
        std::vector<LineVAO::Line> lines;
        for (auto& vertex : mesh->vertices()) {
            indices[vertex] = uint32_t(vertices.size());
            VertexBuffer::Vertex vertexData = {
                    vertex->position(),
                    vec2(0), // TODO
                    vec3(0) // TODO
            };
            vertices.push_back(vertexData);
        }
        for (auto& [_, edge] : mesh->edges()) {
            auto i0 = indices[edge->vertices().first];
            auto i1 = indices[edge->vertices().second];
            lines.push_back({i0, i1});
        }
        _edgeVAO->vertexBuffer()->setVertices(vertices);
        _edgeVAO->setLines(lines);
    }

    {
        // Face VAO
        std::vector<VertexBuffer::Vertex> vertices;
        std::vector<VAO::Triangle> triangles;

        for (auto& [_, face] : mesh->faces()) {
            auto offset = uint32_t(vertices.size());
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

void MeshRenderer::drawFaces(const SP<Operations> &operations, const mat4 &viewMatrix, const Projection &projection) {
    operations->drawSolid.draw(_faceVAO, viewMatrix, projection, vec3(1, 0, 0), vec3(0));
}

void MeshRenderer::drawEdges(const SP<Operations> &operations, const mat4 &viewMatrix, const Projection &projection) {
    operations->drawLine.draw(_edgeVAO, viewMatrix, projection, 1.f, vec3(0));
}

void MeshRenderer::drawVertices(const SP<Operations> &operations, const glm::mat4 &viewMatrix, const Projection &projection) {
    operations->drawCircle.draw(_vertexVAO, viewMatrix, projection, 5.f, vec3(1));
}

}
