#include "MeshVAOGenerator.hpp"
#include "../mesh/Mesh.hpp"
#include "../mesh/algorithm/SplitSharpEdges.hpp"
#include "../document/MeshObject.hpp"
#include "../gl/VAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include <opensubdiv/far/topologyDescriptor.h>
#include <opensubdiv/far/primvarRefiner.h>
#include <opensubdiv/far/patchTableFactory.h>
#include <opensubdiv/far/patchMap.h>
#include <opensubdiv/far/ptexIndices.h>
#include <range/v3/algorithm/copy.hpp>

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
        auto i0 = uint32_t(_mesh.vertices(edge)[0].index);
        auto i1 = uint32_t(_mesh.vertices(edge)[1].index);
        lines.push_back({i0, i1});
    }
    auto edgeIndexBuffer = makeShared<GL::IndexBuffer>();
    edgeIndexBuffer->setLines(lines);
    auto edgeVAO = makeShared<GL::VAO>(_vertexEdgeVertexBuffer, edgeIndexBuffer);
    return edgeVAO;
}

std::unordered_map<Mesh::MaterialHandle, SP<GL::VAO>> MeshVAOGenerator::generateFaceVAOs() const {
    auto mesh = _mesh;
    Mesh::SplitSharpEdges().perform(mesh);

    // calculate normals
    std::vector<glm::vec3> faceNormals(mesh.faceCount());
    for (auto face : mesh.faces()) {
        faceNormals[face.index] = mesh.calculateNormal(face);
    }
    std::vector<glm::vec3> vertexNormals(mesh.vertexCount());
    for (auto vertex : mesh.vertices()) {
        glm::vec3 normalSum(0);
        for (auto vertexFace : mesh.faces(vertex)) {
            normalSum += faceNormals[vertexFace.index];
        }
        normalSum = glm::normalize(normalSum);
        vertexNormals[vertex.index] = normalSum;
    }

    std::vector<Draw::Vertex> uvPointAttributes(mesh.uvPointCount());
    for (auto uv : mesh.uvPoints()) {
        auto v = mesh.vertex(uv);
        uvPointAttributes[uv.index].position = mesh.position(v);
        uvPointAttributes[uv.index].texCoord = mesh.uvPosition(uv);
        uvPointAttributes[uv.index].normal = vertexNormals[v.index];
    }

    std::unordered_map<Mesh::MaterialHandle, std::vector<GL::IndexBuffer::Triangle>> trianglesMap;

    for (auto face : mesh.faces()) {
        auto& triangles = trianglesMap[mesh.material(face)];
        auto& uvPoints = mesh.uvPoints(face);

        auto i0 = uint32_t(uvPoints[0].index);
        for (uint32_t i = 2; i < uvPoints.size(); ++i) {
            auto i1 = uint32_t(uvPoints[i - 1].index);
            auto i2 = uint32_t(uvPoints[i].index);
            triangles.push_back({i0, i1, i2});
        }
    }

    auto vbo = makeShared<GL::VertexBuffer<Draw::Vertex>>();
    vbo->setVertices(uvPointAttributes);

    std::unordered_map<Mesh::MaterialHandle, SP<GL::VAO>> vaos;
    vaos.reserve(trianglesMap.size());
    for (auto [material, triangles] : trianglesMap) {
        auto indexBuffer = makeShared<GL::IndexBuffer>();
        indexBuffer->setTriangles(triangles);
        auto vao = makeShared<GL::VAO>(vbo, indexBuffer);
        vaos.insert({material, vao});
    }
    return vaos;
}

std::unordered_map<Mesh::MaterialHandle, SP<GL::VAO> > MeshVAOGenerator::generateSubdivFaceVAOs(int segmentCount) const {
    using namespace OpenSubdiv;

    struct Vertex {
        // Minimal required interface ----------------------
        Vertex() {}
        Vertex(glm::vec3 point) : point(point) {}

        void Clear(void* = nullptr) {
            point = glm::vec3(0);
        }

        void AddWithWeight(Vertex const & src, float weight) {
            point += weight * src.point;
        }

        glm::vec3 point;
    };

    struct LimitFrame {
        void Clear(void* = nullptr) {
            point = deriv1 = deriv2 = glm::vec3(0);
        }

        void AddWithWeight(Vertex const & src,
                           float weight, float d1Weight, float d2Weight) {

            point += weight * src.point;
            deriv1 += d1Weight * src.point;
            deriv2 += d2Weight * src.point;
        }

        glm::vec3 point;
        glm::vec3 deriv1;
        glm::vec3 deriv2;
    };

    auto mesh = _mesh;
    Mesh::SplitSharpEdges().perform(mesh);
    mesh = mesh.collectGarbage();

    if (mesh.faceCount() == 0) {
        return {};
    }

    std::vector<glm::vec3> origVerts;
    origVerts.reserve(mesh.vertexCount());
    for (auto v : mesh.vertices()) {
        auto pos = mesh.position(v);
        origVerts.push_back(pos);
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
    std::unique_ptr<Far::TopologyRefiner> refiner(Far::TopologyRefinerFactory<Far::TopologyDescriptor>::Create(
        desc,
        Far::TopologyRefinerFactory<Far::TopologyDescriptor>::Options(
            OpenSubdiv::Sdc::SCHEME_CATMARK, options
        )
    ));

    // Adaptively refine the topology with an isolation level capped at 3
    // because the sharpest crease in the shape is 3.0f (in g_creaseweights[])
    int maxIsolation = 3;
    refiner->RefineAdaptive(
        Far::TopologyRefiner::AdaptiveOptions(maxIsolation));

    // Generate a set of Far::PatchTable that we will use to evaluate the
    // surface limit
    Far::PatchTableFactory::Options patchOptions;
    patchOptions.endCapType =
        Far::PatchTableFactory::Options::ENDCAP_GREGORY_BASIS;

    std::unique_ptr<const Far::PatchTable> patchTable(Far::PatchTableFactory::Create(*refiner, patchOptions));

    // Compute the total number of points we need to evaluate patchtable.
    // we use local points around extraordinary features.
    int nRefinerVertices = refiner->GetNumVerticesTotal();
    int nLocalPoints = patchTable->GetNumLocalPoints();

    // Create a buffer to hold the position of the refined verts and
    // local points, then copy the coarse positions at the beginning.
    std::vector<Vertex> verts(nRefinerVertices + nLocalPoints);
    ranges::copy(origVerts, verts.begin());

    // Adaptive refinement may result in fewer levels than maxIsolation.
    int nRefinedLevels = refiner->GetNumLevels();

    // Interpolate vertex primvar data : they are the control vertices
    // of the limit patches (see far_tutorial_0 for details)
    Vertex* src = &verts[0];
    for (int level = 1; level < nRefinedLevels; ++level) {
        Vertex* dst = src + refiner->GetLevel(level-1).GetNumVertices();
        Far::PrimvarRefiner(*refiner).Interpolate(level, src, dst);
        src = dst;
    }

    if (nLocalPoints > 0) {
        // Evaluate local points from interpolated vertex primvars.
        patchTable->ComputeLocalPointValues(&verts[0], &verts[nRefinerVertices]);
    }

    // Create a Far::PatchMap to help locating patches in the table
    Far::PatchMap patchmap(*patchTable);

    // Create a Far::PtexIndices to help find indices of ptex faces.
    Far::PtexIndices ptexIndices(*refiner);

    // create mesh grid
    float pWeights[20], dsWeights[20], dtWeights[20];
    int faceCount = ptexIndices.GetNumFaces();

    std::vector<Draw::Vertex> vertexAttributes;

    for (int faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
        for (int tIndex = 0; tIndex <= segmentCount; ++tIndex) {
            for (int sIndex = 0; sIndex <= segmentCount; ++sIndex) {
                glm::vec2 st = glm::vec2(sIndex, tIndex) / float(segmentCount);

                // Locate the patch corresponding to the face ptex idx and (s,t)
                Far::PatchTable::PatchHandle const * handle =
                    patchmap.FindPatch(faceIndex, st.s, st.t);
                assert(handle);

                // Evaluate the patch weights, identify the CVs and compute the limit frame:
                patchTable->EvaluateBasis(*handle, st.s, st.t, pWeights, dsWeights, dtWeights);

                Far::ConstIndexArray cvs = patchTable->GetPatchVertices(*handle);

                LimitFrame dst;
                dst.Clear();
                for (int cv=0; cv < cvs.size(); ++cv) {
                    dst.AddWithWeight(verts[cvs[cv]], pWeights[cv], dsWeights[cv], dtWeights[cv]);
                }

                Draw::Vertex attrib;
                attrib.position = dst.point;
                attrib.normal = glm::normalize(glm::cross(dst.deriv1, dst.deriv2));
                // TODO: texcoord

                vertexAttributes.push_back(attrib);
            }
        }
    }

    std::vector<GL::IndexBuffer::Triangle> triangles;
    int indexStride = segmentCount + 1;
    for (int faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
        for (int tIndex = 0; tIndex < segmentCount; ++tIndex) {
            for (int sIndex = 0; sIndex < segmentCount; ++sIndex) {
                uint32_t i00 = faceIndex * indexStride * indexStride + tIndex * indexStride + sIndex;
                uint32_t i01 = i00 + 1;
                uint32_t i10 = i00 + indexStride;
                uint32_t i11 = i10 + 1;
                triangles.push_back({i00, i01, i11});
                triangles.push_back({i00, i11, i10});
            }
        }
    }

    auto vbo = makeShared<GL::VertexBuffer<Draw::Vertex>>();
    vbo->setVertices(vertexAttributes);
    auto ibo = makeShared<GL::IndexBuffer>();
    ibo->setTriangles(triangles);
    auto vao = makeShared<GL::VAO>(vbo, ibo);

    return {{Mesh::MaterialHandle(), vao}}; // TODO: support multiple materials
}

} // namespace Viewport
} // namespace Lattice
