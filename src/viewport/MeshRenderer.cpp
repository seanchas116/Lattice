#include "MeshRenderer.hpp"
#include "Operations.hpp"
#include "../document/Mesh.hpp"
#include "../document/MeshItem.hpp"
#include "../gl/VAO.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/PointVAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include <unordered_map>

using namespace glm;

namespace Lattice {

MeshRenderer::MeshRenderer(const SP<MeshItem> &item) : _item(item) {
    auto vbo = std::make_shared<VertexBuffer>();
    _edgeVAO = std::make_shared<LineVAO>(vbo);
    _vertexVAO = std::make_shared<PointVAO>(vbo);
    _faceVAO = std::make_shared<VAO>(vbo);

    // TODO: update mesh when item is changed
    update(item->mesh());
}

void MeshRenderer::update(const SP<Mesh> &mesh) {
    std::unordered_map<SP<MeshVertex>, uint32_t> indices;
    std::vector<VertexBuffer::Vertex> vertices;
    for (auto& vertex : mesh->vertices()) {
        indices[vertex] = uint32_t(vertices.size());
        VertexBuffer::Vertex vertexData = {
            vertex->position(),
            vec2(0), // TODO
            vertex->normal(),
        };
        vertices.push_back(vertexData);
    }
    _edgeVAO->vertexBuffer()->setVertices(vertices);

    {
        std::vector<LineVAO::Line> lines;
        for (auto& [_, edge] : mesh->edges()) {
            auto i0 = indices[edge->vertices()[0]];
            auto i1 = indices[edge->vertices()[1]];
            lines.push_back({i0, i1});
        }
        _edgeVAO->setLines(lines);
    }

    {
        // Face VAO
        std::vector<VAO::Triangle> triangles;

        for (auto& [_, face] : mesh->faces()) {
            auto v0 = face->vertices()[0];
            auto i0 = indices[v0];
            for (uint32_t i = 2; i < uint32_t(face->vertices().size()); ++i) {
                auto v1 = face->vertices()[i - 1];
                auto v2 = face->vertices()[i];
                auto i1 = indices[v1];
                auto i2 = indices[v2];
                triangles.push_back({i0, i1, i2});
            }
        }

        _faceVAO->setTriangles(triangles);
    }
}

void MeshRenderer::drawFaces(const SP<Operations> &operations, const mat4 &viewMatrix, const Projection &projection) {
    operations->drawSolid.draw(_faceVAO, viewMatrix * _item->location().matrix(), projection, vec3(0.8f), vec3(0));
}

void MeshRenderer::drawEdges(const SP<Operations> &operations, const mat4 &viewMatrix, const Projection &projection) {
    operations->drawLine.draw(_edgeVAO, viewMatrix * _item->location().matrix(), projection, 1.f, vec3(0));
}

void MeshRenderer::drawVertices(const SP<Operations> &operations, const glm::mat4 &viewMatrix, const Projection &projection) {
    operations->drawCircle.draw(_vertexVAO, viewMatrix * _item->location().matrix(), projection, 2.f, vec3(1));
}

}
