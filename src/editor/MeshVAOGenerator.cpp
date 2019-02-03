#include "MeshVAOGenerator.hpp"
#include "../document/Mesh.hpp"
#include "../document/MeshItem.hpp"
#include "../oldgl/VAO.hpp"
#include "../oldgl/LineVAO.hpp"
#include "../oldgl/PointVAO.hpp"
#include "../oldgl/VertexBuffer.hpp"

namespace Lattice {
namespace Editor {

MeshVAOGenerator::MeshVAOGenerator(const SP<Document::Mesh> &mesh) :
    _mesh(mesh),
    _vertexBuffer(makeShared<OldGL::VertexBuffer>())
{
    std::vector<OldGL::VertexBuffer::Vertex> vertices;
    for (auto& vertex : mesh->vertices()) {
        for (auto& uvPos : vertex->uvPoints()) {
            _indices[uvPos] = uint32_t(vertices.size());
            OldGL::VertexBuffer::Vertex vertexData = {
                vertex->position(),
                uvPos->position(),
                vertex->normal(),
            };
            vertices.push_back(vertexData);
        }
    }
    _vertexBuffer->setVertices(vertices);
}

SP<OldGL::PointVAO> MeshVAOGenerator::generateVertexVAO() const {
    return makeShared<OldGL::PointVAO>(_vertexBuffer);
}

SP<OldGL::LineVAO> MeshVAOGenerator::generateEdgeVAO() const {
    auto edgeVAO = makeShared<OldGL::LineVAO>(_vertexBuffer);
    std::vector<OldGL::LineVAO::Line> lines;
    for (auto& [_, edge] : _mesh->edges()) {
        auto i0 = _indices.at(*edge->vertices()[0]->uvPoints().begin());
        auto i1 = _indices.at(*edge->vertices()[1]->uvPoints().begin());
        lines.push_back({i0, i1});
    }
    edgeVAO->setLines(lines);
    return edgeVAO;
}

std::unordered_map<SP<Document::MeshMaterial>, SP<OldGL::VAO> > MeshVAOGenerator::generateFaceVAOs() const {
    std::unordered_map<SP<Document::MeshMaterial>, SP<OldGL::VAO> > faceVAOs;

    for (auto& material : _mesh->materials()) {
        auto vao = makeShared<OldGL::VAO>(_vertexBuffer);
        std::vector<OldGL::VAO::Triangle> triangles;
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
