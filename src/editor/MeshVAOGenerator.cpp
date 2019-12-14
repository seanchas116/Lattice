#include "MeshVAOGenerator.hpp"
#include "../document/MeshObject.hpp"
#include "../gl/VAO.hpp"
#include "../gl/VertexBuffer.hpp"
#include <meshlib/Mesh.hpp>
#include <meshlib/algorithm/SplitSharpEdges.hpp>
#include <opensubdiv/far/patchMap.h>
#include <opensubdiv/far/patchTableFactory.h>
#include <opensubdiv/far/primvarRefiner.h>
#include <opensubdiv/far/ptexIndices.h>
#include <opensubdiv/far/topologyDescriptor.h>
#include <range/v3/algorithm/copy.hpp>

using namespace glm;

namespace Lattice {
namespace Editor {

MeshVAOGenerator::MeshVAOGenerator(const Mesh::Mesh &mesh) : _mesh(mesh),
                                                             _vertexEdgeVertexBuffer(makeShared<GL::VertexBuffer<Draw::PointLineVertex>>()) {
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
    Mesh::splitSharpEdges(mesh);

    // calculate normals
    std::vector<glm::vec3> faceNormals(mesh.allFaceCount());
    for (auto face : mesh.faces()) {
        faceNormals[face.index] = mesh.calculateNormal(face);
    }
    std::vector<glm::vec3> vertexNormals(mesh.allVertexCount());
    for (auto vertex : mesh.vertices()) {
        glm::vec3 normalSum(0);
        for (auto vertexFace : mesh.faces(vertex)) {
            normalSum += faceNormals[vertexFace.index];
        }
        normalSum = glm::normalize(normalSum);
        vertexNormals[vertex.index] = normalSum;
    }

    std::vector<Draw::Vertex> uvPointAttributes(mesh.allUVPointCount());
    for (auto uv : mesh.uvPoints()) {
        auto v = mesh.vertex(uv);
        uvPointAttributes[uv.index].position = mesh.position(v);
        uvPointAttributes[uv.index].texCoord = mesh.uvPosition(uv);
        uvPointAttributes[uv.index].normal = vertexNormals[v.index];
    }

    std::unordered_map<Mesh::MaterialHandle, std::vector<GL::IndexBuffer::Triangle>> trianglesMap;

    for (auto face : mesh.faces()) {
        auto &triangles = trianglesMap[mesh.material(face)];
        auto &uvPoints = mesh.uvPoints(face);

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

std::unordered_map<Mesh::MaterialHandle, SP<GL::VAO>> MeshVAOGenerator::generateSubdivFaceVAOs(int segmentCount) const {
    using namespace OpenSubdiv;

    struct Vertex {
        // Minimal required interface ----------------------
        Vertex() {}
        Vertex(glm::vec3 point) : point(point) {}

        void Clear(void * = nullptr) {
            point = glm::vec3(0);
        }

        void AddWithWeight(Vertex const &src, float weight) {
            point += weight * src.point;
        }

        glm::vec3 point;
    };

    struct UV {
        // Minimal required interface ----------------------
        UV() {}
        UV(glm::vec2 point) : point(point) {}

        void Clear(void * = nullptr) {
            point = glm::vec2(0);
        }

        void AddWithWeight(UV const &src, float weight) {
            point += weight * src.point;
        }

        glm::vec2 point;
    };

    auto mesh = _mesh;
    Mesh::splitSharpEdges(mesh);
    mesh = mesh.collectGarbage();

    if (mesh.allFaceCount() == 0) {
        return {};
    }

    std::vector<int> vertsPerFace;
    std::vector<int> faceVerts;
    std::vector<int> faceUVs;
    vertsPerFace.reserve(mesh.allFaceCount());
    for (auto f : mesh.allFaces()) {
        auto &uvPoints = mesh.uvPoints(f);
        vertsPerFace.push_back(int(uvPoints.size()));
        for (auto uv : uvPoints) {
            faceVerts.push_back(mesh.vertex(uv).index);
            faceUVs.push_back(uv.index);
        }
    }

    std::vector<int> creaseVerts;
    std::vector<float> creaseWeights;
    creaseVerts.reserve(mesh.allEdgeCount() * 2);
    creaseWeights.reserve(mesh.allEdgeCount());
    for (auto e : mesh.allEdges()) {
        creaseVerts.push_back(int(mesh.vertices(e)[0].index));
        creaseVerts.push_back(int(mesh.vertices(e)[1].index));
        creaseWeights.push_back(mesh.crease(e));
    }

    // TODO: Cache TopologyRefiner and PatchTable when mesh topology is not changed

    // create topology refiner

    Sdc::Options options;
    options.SetVtxBoundaryInterpolation(Sdc::Options::VTX_BOUNDARY_EDGE_ONLY);
    options.SetFVarLinearInterpolation(Sdc::Options::FVAR_LINEAR_NONE);

    Far::TopologyDescriptor desc;
    desc.numVertices = int(mesh.allVertexCount());
    desc.numFaces = int(mesh.allFaceCount());
    desc.numVertsPerFace = vertsPerFace.data();
    desc.vertIndicesPerFace = faceVerts.data();
    desc.numCreases = int(mesh.allEdgeCount());
    desc.creaseVertexIndexPairs = creaseVerts.data();
    desc.creaseWeights = creaseWeights.data();
    desc.numFVarChannels = 1;
    Far::TopologyDescriptor::FVarChannel uvFVarChannel;
    uvFVarChannel.numValues = int(mesh.allUVPointCount());
    uvFVarChannel.valueIndices = faceUVs.data();
    desc.fvarChannels = &uvFVarChannel;

    // Instantiate a FarTopologyRefiner from the descriptor.
    std::unique_ptr<Far::TopologyRefiner> refiner(Far::TopologyRefinerFactory<Far::TopologyDescriptor>::Create(
        desc,
        Far::TopologyRefinerFactory<Far::TopologyDescriptor>::Options(
            OpenSubdiv::Sdc::SCHEME_CATMARK, options)));

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
    patchOptions.generateFVarTables = true;

    std::unique_ptr<const Far::PatchTable> patchTable(Far::PatchTableFactory::Create(*refiner, patchOptions));

    // Compute the total number of points we need to evaluate patchtable.
    // we use local points around extraordinary features.
    int nRefinerVertices = refiner->GetNumVerticesTotal();
    int nRefinerUVs = refiner->GetNumFVarValuesTotal(0);
    int nLocalPoints = patchTable->GetNumLocalPoints();
    int nLocalPointUV = patchTable->GetNumLocalPointsFaceVarying(0);

    // Create a buffer to hold the position of the refined verts and
    // local points, then copy the coarse positions at the beginning.
    std::vector<Vertex> verts(nRefinerVertices + nLocalPoints);
    for (auto v : mesh.allVertices()) {
        verts[v.index] = mesh.position(v);
    }
    std::vector<UV> uvs(nRefinerUVs + nLocalPointUV);
    for (auto uv : mesh.allUVPoints()) {
        uvs[uv.index] = mesh.uvPosition(uv);
    }

    // Adaptive refinement may result in fewer levels than maxIsolation.
    int nRefinedLevels = refiner->GetNumLevels();

    // Interpolate vertex primvar data : they are the control vertices
    // of the limit patches (see far_tutorial_0 for details)
    {
        Vertex *src = &verts[0];
        for (int level = 1; level < nRefinedLevels; ++level) {
            Vertex *dst = src + refiner->GetLevel(level - 1).GetNumVertices();
            Far::PrimvarRefiner(*refiner).Interpolate(level, src, dst);
            src = dst;
        }
    }
    {
        UV *src = &uvs[0];
        for (int level = 1; level < nRefinedLevels; ++level) {
            UV *dst = src + refiner->GetLevel(level - 1).GetNumFVarValues(0);
            Far::PrimvarRefiner(*refiner).InterpolateFaceVarying(level, src, dst);
            src = dst;
        }
    }

    if (nLocalPoints > 0) {
        // Evaluate local points from interpolated vertex primvars.
        patchTable->ComputeLocalPointValues(&verts[0], &verts[nRefinerVertices]);
        patchTable->ComputeLocalPointValuesFaceVarying(&uvs[0], &uvs[nRefinerUVs], 0);
    }

    // Create a Far::PatchMap to help locating patches in the table
    Far::PatchMap patchmap(*patchTable);

    // Create a Far::PtexIndices to help find indices of ptex faces.
    Far::PtexIndices ptexIndices(*refiner);

    // create mesh grid
    float pWeights[20], dsWeights[20], dtWeights[20];
    int faceCount = ptexIndices.GetNumFaces();

    std::vector<Draw::Vertex> vertexAttributes;
    vertexAttributes.reserve(faceCount * (segmentCount + 1) * (segmentCount + 1));

    for (int faceIndex = 0; faceIndex < faceCount; ++faceIndex) {
        for (int tIndex = 0; tIndex <= segmentCount; ++tIndex) {
            for (int sIndex = 0; sIndex <= segmentCount; ++sIndex) {
                glm::vec2 st = glm::vec2(sIndex, tIndex) / float(segmentCount);

                // Locate the patch corresponding to the face ptex idx and (s,t)
                Far::PatchTable::PatchHandle const *handle =
                    patchmap.FindPatch(faceIndex, st.s, st.t);
                assert(handle);

                Draw::Vertex attrib;

                {
                    // Evaluate the patch weights, identify the CVs and compute the limit frame:
                    patchTable->EvaluateBasis(*handle, st.s, st.t, pWeights, dsWeights, dtWeights);

                    Far::ConstIndexArray cvs = patchTable->GetPatchVertices(*handle);

                    glm::vec3 p(0);
                    glm::vec3 ds(0);
                    glm::vec3 dt(0);

                    for (int cv = 0; cv < cvs.size(); ++cv) {
                        auto src = verts[cvs[cv]].point;
                        p += pWeights[cv] * src;
                        ds += dsWeights[cv] * src;
                        dt += dtWeights[cv] * src;
                    }

                    attrib.position = p;
                    attrib.normal = glm::normalize(glm::cross(ds, dt));
                }

                {
                    patchTable->EvaluateBasisFaceVarying(*handle, st.s, st.t, pWeights, dsWeights, dtWeights, 0);

                    Far::ConstIndexArray cvs = patchTable->GetPatchFVarValues(*handle, 0);

                    glm::vec2 p(0);
                    glm::vec2 ds(0);
                    glm::vec2 dt(0);

                    for (int cv = 0; cv < cvs.size(); ++cv) {
                        auto src = uvs[cvs[cv]].point;
                        p += pWeights[cv] * src;
                        ds += dsWeights[cv] * src;
                        dt += dtWeights[cv] * src;
                    }

                    attrib.texCoord = p;
                }

                vertexAttributes.push_back(attrib);
            }
        }
    }

    std::vector<GL::IndexBuffer::Triangle> triangles;
    triangles.reserve(faceCount * segmentCount * segmentCount * 2);
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

} // namespace Editor
} // namespace Lattice
