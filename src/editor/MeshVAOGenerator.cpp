#include "MeshVAOGenerator.hpp"
#include "../mesh/Mesh.hpp"
#include "../document/MeshObject.hpp"
#include "../gl/VAO.hpp"
#include "../gl/VertexBuffer.hpp"

#include <opensubdiv/far/topologyDescriptor.h>
#include <opensubdiv/far/stencilTable.h>
#include <opensubdiv/far/stencilTableFactory.h>

using namespace glm;

namespace Lattice {
namespace Editor {

MeshVAOGenerator::MeshVAOGenerator(const Mesh::Mesh &mesh) :
    _mesh(mesh),
    _vertexEdgeVertexBuffer(makeShared<GL::VertexBuffer<Draw::PointLineVertex>>())
{
    std::vector<Draw::PointLineVertex> vertexAttributes;
    for (auto vertex : mesh.vertices()) {
        Draw::PointLineVertex vertexAttribute = {
            mesh.position(vertex),
            glm::vec4(0),
            1,
        };
        vertexAttributes.push_back(vertexAttribute);
    }
    _vertexEdgeVertexBuffer->setVertices(vertexAttributes);
}

SP<GL::VAO> MeshVAOGenerator::generateVertexVAO() const {
    return makeShared<GL::VAO>(_vertexEdgeVertexBuffer, GL::Primitive::Point);
}

SP<GL::VAO> MeshVAOGenerator::generateEdgeVAO() const {
    std::vector<GL::IndexBuffer::Line> lines;
    for (auto edge : _mesh.edges()) {
        auto i0 = _mesh.vertices(edge)[0].index;
        auto i1 = _mesh.vertices(edge)[1].index;
        lines.push_back({i0, i1});
    }
    auto edgeIndexBuffer = makeShared<GL::IndexBuffer>();
    edgeIndexBuffer->setLines(lines);
    auto edgeVAO = makeShared<GL::VAO>(_vertexEdgeVertexBuffer, edgeIndexBuffer);
    return edgeVAO;
}

std::unordered_map<uint32_t, SP<GL::VAO>> MeshVAOGenerator::generateFaceVAOs() const {
    std::vector<Draw::Vertex> vertexAttributes;
    std::unordered_map<uint32_t, std::vector<GL::IndexBuffer::Triangle>> trianglesMap;

    for (auto face : _mesh.faces()) {
        auto faceNormal = _mesh.calculateNormal(face);

        auto addPoint = [&](Mesh::FaceHandle face, uint32_t indexInFace) {
            auto p = _mesh.uvPoints(face)[indexInFace];
            Draw::Vertex attrib;
            attrib.position = _mesh.position(_mesh.vertex(p));
            attrib.texCoord = _mesh.uvPosition(p);
            attrib.normal = faceNormal; // TODO: calculate smooth edge normals

            auto index = uint32_t(vertexAttributes.size());
            vertexAttributes.push_back(attrib);
            return index;
        };

        auto& triangles = trianglesMap[_mesh.material(face)];

        auto i0 = addPoint(face, 0);
        auto vertexCount = _mesh.uvPoints(face).size();
        for (uint32_t i = 2; i < vertexCount; ++i) {
            auto i1 = addPoint(face, i - 1);
            auto i2 = addPoint(face, i);
            triangles.push_back({i0, i1, i2});
        }
    }

    auto vbo = makeShared<GL::VertexBuffer<Draw::Vertex>>();
    vbo->setVertices(vertexAttributes);

    std::unordered_map<uint32_t, SP<GL::VAO>> vaos;
    vaos.reserve(trianglesMap.size());
    for (auto [material, triangles] : trianglesMap) {
        auto indexBuffer = makeShared<GL::IndexBuffer>();
        indexBuffer->setTriangles(triangles);
        auto vao = makeShared<GL::VAO>(vbo, indexBuffer);
        vaos.insert({material, vao});
    }
    return vaos;
}

std::unordered_map<uint32_t, SP<GL::VAO> > MeshVAOGenerator::generateSubdivFaceVAOs(int level) const {
    using namespace OpenSubdiv;

    auto mesh = _mesh.collectGarbage();

    std::vector<float> verts;
    verts.reserve(mesh.vertexCount() * 3);
    for (auto v : mesh.vertices()) {
        auto pos = mesh.position(v);
        verts.push_back(pos.x);
        verts.push_back(pos.y);
        verts.push_back(pos.z);
    }

    std::vector<int> vertsPerFace;
    vertsPerFace.reserve(mesh.faceCount());
    for (auto f : mesh.faces()) {
        vertsPerFace.push_back(int(mesh.vertices(f).size()));
    }

    std::vector<int> faceVerts;
    for (auto f : mesh.faces()) {
        for (auto v : mesh.vertices(f)) {
            faceVerts.push_back(int(v.index));
        }
    }

    std::vector<int> creaseVerts;
    creaseVerts.reserve(mesh.edgeCount() * 2);
    for (auto e : mesh.edges()) {
        creaseVerts.push_back(int(mesh.vertices(e)[0].index));
        creaseVerts.push_back(int(mesh.vertices(e)[1].index));
    }

    std::vector<float> creaseWeights;
    creaseWeights.reserve(mesh.edgeCount());
    for (auto e : mesh.edges()) {
        creaseWeights.push_back(mesh.crease(e));
    }

    // create topology refiner

    Sdc::Options options;
    options.SetVtxBoundaryInterpolation(Sdc::Options::VTX_BOUNDARY_EDGE_ONLY);

    Far::TopologyDescriptor desc;
    desc.numVertices = int(mesh.vertexCount());
    desc.numFaces = int(mesh.faceCount());
    desc.numVertsPerFace = vertsPerFace.data();
    desc.vertIndicesPerFace = faceVerts.data();
    desc.numCreases = int(mesh.edgeCount());
    desc.creaseVertexIndexPairs = creaseVerts.data();
    desc.creaseWeights = creaseWeights.data();

    // Instantiate a FarTopologyRefiner from the descriptor.
    auto refiner = Far::TopologyRefinerFactory<Far::TopologyDescriptor>::Create(
        desc,
        Far::TopologyRefinerFactory<Far::TopologyDescriptor>::Options(
            OpenSubdiv::Sdc::SCHEME_CATMARK, options
        )
    );

    Q_UNUSED(refiner);

    // WIP
    Q_UNUSED(level);
    return {};
}

} // namespace Viewport
} // namespace Lattice
