#include "MeshVAOGenerator.hpp"
#include "../mesh/Mesh.hpp"
#include "../document/MeshItem.hpp"
#include "../gl/VAO.hpp"
#include "../gl/VertexBuffer.hpp"

namespace Lattice {
namespace Editor {

MeshVAOGenerator::MeshVAOGenerator(const SP<Mesh::Mesh> &mesh) :
    _mesh(mesh),
    _vertexBuffer(makeShared<GL::VertexBuffer<GL::Vertex>>())
{
    std::vector<GL::Vertex> vertices;
    for (auto& vertex : mesh->vertices()) {
        for (auto& uvPos : vertex->uvPoints()) {
            _foreIndices[uvPos] = uint32_t(vertices.size());
            GL::Vertex foreVertexData = {
                vertex->position(),
                uvPos->position(),
                vertex->normal(),
            };
            vertices.push_back(foreVertexData);

            _backIndices[uvPos] = uint32_t(vertices.size());
            GL::Vertex backVertexData = {
                vertex->position(),
                uvPos->position(),
                -vertex->normal(),
            };
            vertices.push_back(backVertexData);
        }
    }
    _vertexBuffer->setVertices(vertices);
}

SP<GL::VAO> MeshVAOGenerator::generateVertexVAO() const {
    return makeShared<GL::VAO>(_vertexBuffer, GL::Primitive::Point);
}

SP<GL::VAO> MeshVAOGenerator::generateEdgeVAO() const {
    std::vector<GL::IndexBuffer::Line> lines;
    for (auto& [_, edge] : _mesh->edges()) {
        auto i0 = _foreIndices.at(*edge->vertices()[0]->uvPoints().begin());
        auto i1 = _foreIndices.at(*edge->vertices()[1]->uvPoints().begin());
        lines.push_back({i0, i1});
    }
    auto edgeIndexBuffer = makeShared<GL::IndexBuffer>();
    edgeIndexBuffer->setLines(lines);
    auto edgeVAO = makeShared<GL::VAO>(_vertexBuffer, edgeIndexBuffer);
    return edgeVAO;
}

std::unordered_map<SP<Mesh::Material>, SP<GL::VAO>> MeshVAOGenerator::generateFaceVAOs() const {
    std::unordered_map<SP<Mesh::Material>, SP<GL::VAO>> faceVAOs;

    for (auto& material : _mesh->materials()) {
        std::vector<GL::IndexBuffer::Triangle> triangles;
        for (auto& face : material->faces()) {
            auto v0 = face->uvPoints()[0];

            { // fore
                auto i0 = _foreIndices.at(v0.get());
                for (uint32_t i = 2; i < uint32_t(face->vertices().size()); ++i) {
                    auto v1 = face->uvPoints()[i - 1];
                    auto v2 = face->uvPoints()[i];
                    auto i1 = _foreIndices.at(v1.get());
                    auto i2 = _foreIndices.at(v2.get());
                    triangles.push_back({i0, i1, i2});
                }
            }

            /*
            { // back
                auto i0 = _backIndices.at(v0.get());
                for (uint32_t i = 2; i < uint32_t(face->vertices().size()); ++i) {
                    auto v1 = face->uvPoints()[i - 1];
                    auto v2 = face->uvPoints()[i];
                    auto i1 = _backIndices.at(v1.get());
                    auto i2 = _backIndices.at(v2.get());
                    triangles.push_back({i0, i2, i1});
                }
            }
            */
        }
        auto indexBuffer = makeShared<GL::IndexBuffer>();
        indexBuffer->setTriangles(triangles);
        auto vao = makeShared<GL::VAO>(_vertexBuffer, indexBuffer);
        faceVAOs.insert({material, vao});
    }

    return faceVAOs;
}

} // namespace Viewport
} // namespace Lattice
