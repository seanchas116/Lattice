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

namespace Lattice::Viewport {

MeshRenderer::MeshRenderer(const SP<Document::MeshItem> &item) : _item(item) {
    _vbo = std::make_shared<GL::VertexBuffer>();
    _edgeVAO = std::make_shared<GL::LineVAO>(_vbo);
    _vertexVAO = std::make_shared<GL::PointVAO>(_vbo);

    // TODO: update mesh when item is changed
    update(item->mesh());
}

void MeshRenderer::update(const SP<Document::Mesh> &mesh) {
    std::unordered_map<SP<Document::MeshUVPoint>, uint32_t> indices;
    std::vector<GL::VertexBuffer::Vertex> vertices;
    for (auto& vertex : mesh->vertices()) {
        for (auto& uvPos : vertex->uvPoints()) {
            indices[uvPos] = uint32_t(vertices.size());
            GL::VertexBuffer::Vertex vertexData = {
                vertex->position(),
                uvPos->position(),
                vertex->normal(),
            };
            vertices.push_back(vertexData);
        }
    }
    _vbo->setVertices(vertices);

    {
        std::vector<GL::LineVAO::Line> lines;
        for (auto& [_, edge] : mesh->edges()) {
            auto i0 = indices[*edge->vertices()[0]->uvPoints().begin()];
            auto i1 = indices[*edge->vertices()[1]->uvPoints().begin()];
            lines.push_back({i0, i1});
        }
        _edgeVAO->setLines(lines);
    }

    {
        // Face VAO

        for (auto& material : mesh->materials()) {
            auto vao = std::make_shared<GL::VAO>(_vbo);
            std::vector<GL::VAO::Triangle> triangles;
            for (auto& face : material->faces()) {
                auto v0 = face->uvPoints()[0];
                auto i0 = indices[v0];
                for (uint32_t i = 2; i < uint32_t(face->vertices().size()); ++i) {
                    auto v1 = face->uvPoints()[i - 1];
                    auto v2 = face->uvPoints()[i];
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

void MeshRenderer::drawFaces(const SP<Operations> &operations, const Camera &camera) {
    for (auto& [material, vao] : _faceVAOs) {
        //operations->drawSolid.draw(vao, viewMatrix * _item->location().matrix(), projection, material->baseColor(), vec3(0));
        operations->drawMaterial.draw(vao, _item->location().matrix(), camera, material);
    }
}

void MeshRenderer::drawEdges(const SP<Operations> &operations, const Camera &camera) {
    operations->drawLine.draw(_edgeVAO, _item->location().matrix(), camera, 1.0, dvec3(0));
}

void MeshRenderer::drawVertices(const SP<Operations> &operations, const Camera &camera) {
    operations->drawCircle.draw(_vertexVAO, _item->location().matrix(), camera, 4.0, dvec3(0));
}

}
