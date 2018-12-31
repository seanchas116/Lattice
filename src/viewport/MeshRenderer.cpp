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
    _vbo = std::make_shared<VertexBuffer>();
    _edgeVAO = std::make_shared<LineVAO>(_vbo);
    _vertexVAO = std::make_shared<PointVAO>(_vbo);

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
            vertex->texCoord(),
            vertex->normal(),
        };
        vertices.push_back(vertexData);
    }
    _vbo->setVertices(vertices);

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

        for (auto& material : mesh->materials()) {
            auto vao = std::make_shared<VAO>(_vbo);
            std::vector<VAO::Triangle> triangles;
            for (auto& face : material->faces()) {
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
            vao->setTriangles(triangles);
            _faceVAOs[material] = vao;
        }
    }
}

void MeshRenderer::drawFaces(const SP<Operations> &operations, const mat4 &viewMatrix, const Camera &projection) {
    for (auto& [material, vao] : _faceVAOs) {
        //operations->drawSolid.draw(vao, viewMatrix * _item->location().matrix(), projection, material->baseColor(), vec3(0));
        operations->drawMaterial.draw(vao, viewMatrix * _item->location().matrix(), projection, material);
    }
}

void MeshRenderer::drawEdges(const SP<Operations> &operations, const mat4 &viewMatrix, const Camera &projection) {
    operations->drawLine.draw(_edgeVAO, viewMatrix * _item->location().matrix(), projection, 1.f, vec3(0));
}

void MeshRenderer::drawVertices(const SP<Operations> &operations, const glm::mat4 &viewMatrix, const Camera &projection) {
    operations->drawCircle.draw(_vertexVAO, viewMatrix * _item->location().matrix(), projection, 4.f, vec3(0));
}

}
