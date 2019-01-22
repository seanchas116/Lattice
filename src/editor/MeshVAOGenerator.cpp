#include "MeshVAOGenerator.hpp"
#include "../document/Mesh.hpp"
#include "../document/MeshItem.hpp"
#include "../gl/VAO.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/PointVAO.hpp"
#include "../gl/VertexBuffer.hpp"

namespace Lattice {
namespace Editor {

MeshVAOGenerator::MeshVAOGenerator(const SP<Document::Mesh> &mesh) :
    _mesh(mesh),
    _vertexBuffer(makeShared<GL::VertexBuffer>())
{
    std::vector<GL::VertexBuffer::Vertex> vertices;
    for (auto& vertex : mesh->vertices()) {
        for (auto& uvPos : vertex->uvPoints()) {
            _indices[uvPos] = uint32_t(vertices.size());
            GL::VertexBuffer::Vertex vertexData = {
                vertex->position(),
                uvPos->position(),
                vertex->normal(),
            };
            vertices.push_back(vertexData);
        }
    }
    _vertexBuffer->setVertices(vertices);
}

SP<GL::PointVAO> MeshVAOGenerator::generateVertexVAO() const {
    return makeShared<GL::PointVAO>(_vertexBuffer);
}

SP<GL::LineVAO> MeshVAOGenerator::generateEdgeVAO() const {
    auto edgeVAO = makeShared<GL::LineVAO>(_vertexBuffer);
    std::vector<GL::LineVAO::Line> lines;
    for (auto& [_, edge] : _mesh->edges()) {
        auto i0 = _indices.at(*edge->vertices()[0]->uvPoints().begin());
        auto i1 = _indices.at(*edge->vertices()[1]->uvPoints().begin());
        lines.push_back({i0, i1});
    }
    edgeVAO->setLines(lines);
    return edgeVAO;
}

std::unordered_map<SP<Document::MeshMaterial>, SP<GL::VAO> > MeshVAOGenerator::generateFaceVAOs() const {
    std::unordered_map<SP<Document::MeshMaterial>, SP<GL::VAO> > faceVAOs;

    for (auto& material : _mesh->materials()) {
        auto vao = makeShared<GL::VAO>(_vertexBuffer);
        std::vector<GL::VAO::Triangle> triangles;
        for (auto& face : material->faces()) {
            auto v0 = face->uvPoints()[0];
            auto i0 = _indices.at(v0.get());
            for (uint32_t i = 2; i < uint32_t(face->vertices().size()); ++i) {
                auto v1 = face->uvPoints()[i - 1];
                auto v2 = face->uvPoints()[i];
                auto i1 = _indices.at(v1.get());
                auto i2 = _indices.at(v2.get());
                triangles.push_back({i0, i1, i2});
            }
        }
        vao->setTriangles(triangles);
        faceVAOs.insert({material, vao});
    }

    return faceVAOs;
}

} // namespace Viewport
} // namespace Lattice
