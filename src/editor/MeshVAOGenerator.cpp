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
    // TODO: build more efficient VBO
    std::unordered_map<SP<Mesh::Material>, SP<GL::VAO>> faceVAOs;
    auto faceVBO = makeShared<GL::VertexBuffer<GL::Vertex>>();
    std::vector<GL::Vertex> faceAttributes;

    auto addPoint = [&](const SP<Mesh::Face>& face, const SP<Mesh::UVPoint>& p) {
        GL::Vertex attrib;
        attrib.position = p->vertex()->position();
        attrib.texCoord = p->position();
        attrib.normal = p->vertex()->normalForFace(face);

        auto index = uint32_t(faceAttributes.size());
        faceAttributes.push_back(attrib);
        return index;
    };

    std::vector<GL::IndexBuffer::Triangle> pickTriangles;

    for (auto& material : _mesh->materials()) {
        std::vector<GL::IndexBuffer::Triangle> triangles;
        for (auto& face : material->faces()) {
            auto i0 = addPoint(face->sharedFromThis(), face->uvPoints()[0]);
            for (uint32_t i = 2; i < uint32_t(face->vertices().size()); ++i) {
                auto i1 = addPoint(face->sharedFromThis(), face->uvPoints()[i - 1]);
                auto i2 = addPoint(face->sharedFromThis(), face->uvPoints()[i]);
                triangles.push_back({i0, i1, i2});
                pickTriangles.push_back({i0, i1, i2});
            }
        }
        auto indexBuffer = makeShared<GL::IndexBuffer>();
        indexBuffer->setTriangles(triangles);
        auto vao = makeShared<GL::VAO>(faceVBO, indexBuffer);
        faceVAOs.insert({material, vao});
    }
    faceVBO->setVertices(faceAttributes);

    return faceVAOs;
}

} // namespace Viewport
} // namespace Lattice
