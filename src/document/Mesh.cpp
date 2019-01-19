#include "Mesh.hpp"
#include "../support/Mod.hpp"

using namespace glm;

namespace Lattice::Document {

std::vector<SP<MeshEdge> > MeshVertex::edges() const {
    std::vector<SP<MeshEdge>> edges;
    for (auto& e : _edges) {
        edges.push_back(e->sharedFromThis());
    }
    return edges;
}

std::vector<SP<MeshFace> > MeshVertex::faces() const {
    std::vector<SP<MeshFace>> faces;
    for (auto& f : _faces) {
        faces.push_back(f->sharedFromThis());
    }
    return faces;
}

glm::vec3 MeshVertex::normal() const {
    glm::vec3 normalSum(0);
    for (auto& face : _faces) {
        normalSum += face->normal();
    }
    return normalize(normalSum / float(_faces.size()));
}

SP<MeshUVPoint> MeshVertex::addUVPoint() {
    auto uv = makeShared<MeshUVPoint>();
    _uvPoints.insert(uv);
    uv->_vertex = this;
    return uv;
}

MeshEdge::MeshEdge(const std::array<SP<MeshVertex>, 2> &vertices) : _vertices(vertices) {
    vertices[0]->_edges.insert(this);
    vertices[1]->_edges.insert(this);
}

MeshEdge::~MeshEdge() {
    _vertices[0]->_edges.erase(this);
    _vertices[1]->_edges.erase(this);
}

SP<MeshVertex> MeshUVPoint::vertex() const {
    return _vertex->sharedFromThis();
}

std::vector<SP<MeshUVEdge> > MeshUVPoint::uvEdges() const {
    std::vector<SP<MeshUVEdge>> uvEdges;
    for (auto& e : _uvEdges) {
        uvEdges.push_back(e->sharedFromThis());
    }
    return uvEdges;
}

std::vector<SP<MeshFace> > MeshUVPoint::faces() const {
    std::vector<SP<MeshFace>> faces;
    for (auto& f : _faces) {
        faces.push_back(f->sharedFromThis());
    }
    return faces;
}

MeshUVEdge::MeshUVEdge(const std::array<SP<MeshUVPoint>, 2> &points) : _points(points) {
    points[0]->_uvEdges.insert(this);
    points[1]->_uvEdges.insert(this);
}

MeshUVEdge::~MeshUVEdge() {
    _points[0]->_uvEdges.erase(this);
    _points[1]->_uvEdges.erase(this);
}

std::vector<SP<MeshFace> > MeshUVEdge::faces() const {
    std::vector<SP<MeshFace>> faces;
    for (auto& f : _faces) {
        faces.push_back(f->sharedFromThis());
    }
    return faces;
}

std::vector<SP<MeshFace> > MeshEdge::faces() const {
    std::vector<SP<MeshFace>> faces;
    for (auto& f : _faces) {
        faces.push_back(f->sharedFromThis());
    }
    return faces;
}

MeshFace::MeshFace(const std::vector<SP<MeshVertex> > &vertices, const std::vector<SP<MeshEdge> > &edges,
                   const std::vector<SP<MeshUVPoint> > &uvPoints, const std::vector<SP<MeshUVEdge> > &uvEdges,
                   const SP<MeshMaterial> &material) :
    _vertices(vertices),
    _edges(edges),
    _uvPoints(uvPoints),
    _uvEdges(uvEdges),
    _material(material)
{
}

glm::vec3 MeshFace::normal() const {
    if (_vertices.size() == 3) {
        return normalize(cross(_vertices[1]->position() - _vertices[0]->position(), _vertices[2]->position() - _vertices[0]->position()));
    }

    // find average vertex normal
    glm::vec3 normalSum(0);
    size_t count = _vertices.size();

    for (size_t i = 0; i < count; ++i) {
        auto prev = _vertices[mod(i - 1, count)]->position();
        auto curr = _vertices[i]->position();
        auto next = _vertices[mod(i + 1, count)]->position();
        auto normal = normalize(cross(next - curr, prev - curr));
        normalSum += normal;
    }

    return normalize(normalSum / float(count));
}

Mesh::Mesh() {
}

SP<MeshVertex> Mesh::addVertex(glm::vec3 position) {
    auto vertex = makeShared<MeshVertex>();
    vertex->setPosition(position);
    //vertex->setTexCoord(texCoord);
    _vertices.insert(vertex);
    return vertex;
}

SP<MeshEdge> Mesh::addEdge(const std::array<SP<MeshVertex>, 2> &vertices) {
    auto it = _edges.find(vertices);
    if (it != _edges.end()) {
        return it->second;
    }

    auto edge = makeShared<MeshEdge>(vertices);
    _edges.insert({vertices, edge});
    return edge;
}

SP<MeshUVPoint> Mesh::addUVPoint(const SP<MeshVertex> &vertex, vec2 position) {
    auto uv = vertex->addUVPoint();
    uv->setPosition(position);
    return uv;
}

SP<MeshUVEdge> Mesh::addUVEdge(const std::array<SP<MeshUVPoint>, 2> &uvPoints) {
    auto it = _uvEdges.find(uvPoints);
    if (it != _uvEdges.end()) {
        return it->second;
    }

    auto uvEdge = makeShared<MeshUVEdge>(uvPoints);
    _uvEdges.insert({uvPoints, uvEdge});
    return uvEdge;
}

SP<MeshFace> Mesh::addFace(const std::vector<SP<MeshUVPoint> > &uvPoints, const SP<MeshMaterial> &material) {
    Q_ASSERT(2 <= uvPoints.size());

    std::vector<SP<MeshVertex>> vertices;
    for (auto& uv : uvPoints) {
        vertices.push_back((uv->vertex()));
    }

    auto sortedVertices = vertices;
    std::sort(sortedVertices.begin(), sortedVertices.end());

    auto it = _faces.find(sortedVertices);
    if (it != _faces.end()) {
        return it->second;
    }

    std::vector<SP<MeshEdge>> edges;
    std::vector<SP<MeshUVEdge>> uvEdges;
    for (size_t i = 0; i < vertices.size(); ++i) {
        edges.push_back(addEdge({vertices[i], vertices[(i + 1) % vertices.size()]}));
        uvEdges.push_back(addUVEdge({uvPoints[i], uvPoints[(i + 1) % vertices.size()]}));
    }

    auto face = makeShared<MeshFace>(vertices, edges, uvPoints, uvEdges, material);

    for (auto& v : vertices) {
        v->_faces.insert(face.get());
    }
    for (auto& e : edges) {
        e->_faces.insert(face.get());
    }
    for (auto& uv : uvPoints) {
        uv->_faces.insert(face.get());
    }
    for (auto& uvEdge : uvEdges) {
        uvEdge->_faces.insert(face.get());
    }
    material->_faces.insert(face.get());

    _faces.insert({vertices, face});
    return face;
}

SP<MeshMaterial> Mesh::addMaterial() {
    auto material = makeShared<MeshMaterial>();
    _materials.push_back(material);
    return material;
}

void Mesh::removeFace(const SP<MeshFace> &face) {
    // TODO: erase face with O(1) way
    auto it = std::find_if(_faces.begin(), _faces.end(), [&](const auto& pair) { return pair.second == face; });
    if (it == _faces.end()) {
        return;
    }
    for (auto& v : face->_vertices) {
        v->_faces.erase(face.get());
    }
    for (auto& e : face->_edges) {
        e->_faces.erase(face.get());
    }
    for (auto& uv : face->_uvPoints) {
        uv->_faces.erase(face.get());
    }
    for (auto& uvEdge : face->_uvEdges) {
        uvEdge->_faces.erase(face.get());
    }
    face->_material->_faces.erase(face.get());

    face->_vertices.clear();
    face->_edges.clear();
    face->_uvPoints.clear();
    face->_uvEdges.clear();
    // TODO: clear material

    _faces.erase(it);
}

void Mesh::addPlane(dvec3 center, dvec2 size, int normalAxis, const SP<MeshMaterial> &material) {
    std::vector<SP<MeshUVPoint>> uvPoints;
    std::array<dvec2, 4> uvs = {dvec2(0, 0), dvec2(1, 0), dvec2(1, 1), dvec2(0, 1)};

    for (dvec2 uv : uvs) {
        dvec3 pos = center;
        pos[(normalAxis + 1) % 3] += size.x * (uv.x - 0.5);
        pos[(normalAxis + 2) % 3] += size.y * (uv.y - 0.5);
        auto v = addVertex(pos);
        auto uvPoint = addUVPoint(v, uv);
        uvPoints.push_back(uvPoint);
    }
    addFace(uvPoints, material);
}

void Mesh::addCube(glm::dvec3 minPos, glm::dvec3 maxPos, const SP<MeshMaterial> &material) {
    //   2    3
    // 6    7
    //   0    1
    // 4    5

    auto v0 = addUVPoint(addVertex(vec3(minPos.x, minPos.y, minPos.z)), vec2(0));
    auto v1 = addUVPoint(addVertex(vec3(maxPos.x, minPos.y, minPos.z)), vec2(0));
    auto v2 = addUVPoint(addVertex(vec3(minPos.x, maxPos.y, minPos.z)), vec2(0));
    auto v3 = addUVPoint(addVertex(vec3(maxPos.x, maxPos.y, minPos.z)), vec2(0));
    auto v4 = addUVPoint(addVertex(vec3(minPos.x, minPos.y, maxPos.z)), vec2(0));
    auto v5 = addUVPoint(addVertex(vec3(maxPos.x, minPos.y, maxPos.z)), vec2(0));
    auto v6 = addUVPoint(addVertex(vec3(minPos.x, maxPos.y, maxPos.z)), vec2(0));
    auto v7 = addUVPoint(addVertex(vec3(maxPos.x, maxPos.y, maxPos.z)), vec2(0));

    auto f0 = addFace({v0, v4, v6, v2}, material);
    auto f1 = addFace({v1, v3, v7, v5}, material);
    auto f2 = addFace({v0, v1, v5, v4}, material);
    auto f3 = addFace({v2, v6, v7, v3}, material);
    auto f4 = addFace({v0, v2, v3, v1}, material);
    auto f5 = addFace({v4, v5, v7, v6}, material);
}

void Mesh::addCircle(glm::dvec3 center, double radius, int segmentCount, Mesh::CircleFill fill, int normalAxis, const SP<MeshMaterial> &material) {
    Q_UNUSED(fill);
    // TODO: use CircleFill
    std::vector<SP<MeshUVPoint>> uvPoints;

    double angleStep = M_PI * 2 / segmentCount;
    for (int i = 0 ; i < segmentCount; ++i) {
        double angle = angleStep * i;
        dvec3 offset(0);
        offset[(normalAxis + 1) % 3] = cos(angle);
        offset[(normalAxis + 2) % 3] = sin(angle);
        dvec3 pos = center + offset * radius;
        uvPoints.push_back(addUVPoint(addVertex(pos), vec2(0)));
    }

    addFace(uvPoints, material);
}

void Mesh::addSphere(dvec3 center, double radius, int segmentCount, int ringCount, int axis, const SP<MeshMaterial> &material) {
    std::vector<std::vector<SP<MeshUVPoint>>> uvPointMatrix;

    for (int ring = 0; ring < ringCount - 1; ++ring) {
        std::vector<SP<MeshUVPoint>> uvPoints;

        double longitude = M_PI * (ring + 1 - ringCount * 0.5) / ringCount;

        for (int i = 0; i < segmentCount; ++i) {
            double latitude = M_PI * 2.0 * (double(i) / segmentCount);
            dvec3 offset;
            offset[axis] = sin(longitude);
            offset[(axis + 1) % 3] = cos(latitude) * cos(longitude);
            offset[(axis + 2) % 3] = sin(latitude) * cos(longitude);
            dvec3 pos = center + offset * radius;
            uvPoints.push_back(addUVPoint(addVertex(pos), vec2(0)));
        }

        uvPointMatrix.push_back(std::move(uvPoints));
    }

    auto bottom = addUVPoint(addVertex(center + dvec3(0, -radius, 0)), vec2(0));
    auto top = addUVPoint(addVertex(center + dvec3(0, radius, 0)), vec2(0));

    for (int i = 0; i < segmentCount; ++i) {
        int next = (i + 1) % segmentCount;
        addFace({bottom, uvPointMatrix[0][next], uvPointMatrix[0][i]}, material);

        for (int ring = 0; ring < ringCount - 2; ++ring) {
            addFace({uvPointMatrix[ring][i], uvPointMatrix[ring][next], uvPointMatrix[ring+1][next], uvPointMatrix[ring+1][i]}, material);
        }

        addFace({uvPointMatrix[ringCount - 2][i], uvPointMatrix[ringCount - 2][next], top}, material);
    }
}

void Mesh::addCone(dvec3 center, double radius, double height, int segmentCount, int axis, const SP<MeshMaterial> &material) {
    std::vector<SP<MeshUVPoint>> uvPoints;

    double angleStep = M_PI * 2 / segmentCount;
    for (int i = 0 ; i < segmentCount; ++i) {
        double angle = angleStep * i;
        dvec3 offset(0);
        offset[(axis + 1) % 3] = cos(angle);
        offset[(axis + 2) % 3] = sin(angle);
        dvec3 pos = center + offset * radius;
        auto v = addUVPoint(addVertex(pos), vec2(0));
        uvPoints.push_back(v);
    }

    addFace(uvPoints, material);

    dvec3 topPosition = center;
    topPosition[axis] += height;

    auto top = addUVPoint(addVertex(topPosition), vec2(0));

    for (int i = 0; i < segmentCount; ++i) {
        auto v0 = uvPoints[i];
        auto v1 = uvPoints[(i + 1) % segmentCount];
        addFace({v0, v1, top}, material);
    }
}

void Mesh::merge(const SP<const Mesh> &other) {
    std::unordered_map<SP<MeshMaterial>, SP<MeshMaterial>> otherToNewMaterials;
    std::unordered_map<SP<MeshVertex>, SP<MeshVertex>> otherToNewVertices;
    std::unordered_map<SP<MeshUVPoint>, SP<MeshUVPoint>> otherToNewUVPoints;

    for (auto& otherMaterial : other->materials()) {
        auto m = addMaterial();
        // TODO: copy material parameters
        otherToNewMaterials.insert({otherMaterial, m});
    }

    for (auto& otherV : other->vertices()) {
        auto v = addVertex(otherV->position());
        otherToNewVertices.insert({otherV, v});
        for (auto& otherUV : otherV->uvPoints()) {
            auto uv = addUVPoint(v, otherUV->position());
            otherToNewUVPoints.insert({otherUV, uv});
        }
    }

    for (auto& [_, otherEdge] : other->edges()) {
        Q_UNUSED(_)
        addEdge({otherToNewVertices.at(otherEdge->vertices()[0]), otherToNewVertices.at(otherEdge->vertices()[1])});
    }
    for (auto& [_, otherUVEdge] : other->uvEdges()) {
        Q_UNUSED(_)
        addUVEdge({otherToNewUVPoints.at(otherUVEdge->points()[0]), otherToNewUVPoints.at(otherUVEdge->points()[1])});
    }
    for (auto& [_, otherFace] : other->faces()) {
        Q_UNUSED(_)
        std::vector<SP<MeshUVPoint>> uvPoints;
        for (auto& otherUV : otherFace->uvPoints()) {
            uvPoints.push_back(otherToNewUVPoints.at(otherUV));
        }
        addFace(uvPoints, otherToNewMaterials.at(otherFace->material()));
    }
}

SP<Mesh> Mesh::clone() const {
    auto newMesh = makeShared<Mesh>();
    newMesh->merge(sharedFromThis());
    return newMesh;
}

Box<float> Mesh::boundingBox() const {
    vec3 minPos(INFINITY);
    vec3 maxPos(-INFINITY);
    for (auto& v : _vertices) {
        vec3 p = v->position();
        minPos = min(p, minPos);
        maxPos = max(p, maxPos);
    }

    return {minPos, maxPos};
}

std::vector<SP<MeshFace>> MeshMaterial::faces() const {
    std::vector<SP<MeshFace>> faces;
    faces.reserve(_faces.size());
    for (auto& f : _faces) {
        faces.push_back(f->sharedFromThis());
    }
    return faces;
}

} // namespace Lattice
