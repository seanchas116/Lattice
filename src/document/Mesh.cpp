#include "Mesh.hpp"
#include "../support/Mod.hpp"

using namespace glm;

namespace Lattice {

std::vector<SP<MeshEdge> > MeshVertex::edges() const {
    std::vector<SP<MeshEdge>> edges;
    for (auto& e : _edges) {
        edges.push_back(e->shared_from_this());
    }
    return edges;
}

std::vector<SP<MeshFace> > MeshVertex::faces() const {
    std::vector<SP<MeshFace>> faces;
    for (auto& f : _faces) {
        faces.push_back(f->shared_from_this());
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

MeshEdge::MeshEdge(const std::array<SP<MeshVertex>, 2> &vertices) : _vertices(vertices) {
    vertices[0]->_edges.insert(this);
    vertices[1]->_edges.insert(this);
}

MeshEdge::~MeshEdge() {
    _vertices[0]->_edges.erase(this);
    _vertices[1]->_edges.erase(this);
}

std::vector<SP<MeshFace> > MeshEdge::faces() const {
    std::vector<SP<MeshFace>> faces;
    for (auto& f : _faces) {
        faces.push_back(f->shared_from_this());
    }
    return faces;
}

MeshFace::MeshFace(const std::vector<SP<MeshVertex> > &vertices, const std::vector<SP<MeshEdge> > &edges, const SP<MeshMaterial> &material) :
    _vertices(vertices),
    _edges(edges),
    _material(material)
{
    for (auto& v : vertices) {
        v->_faces.insert(this);
    }
    for (auto& e : edges) {
        e->_faces.insert(this);
    }
    material->_faces.insert(this);
}

MeshFace::~MeshFace() {
    for (auto& v : _vertices) {
        v->_faces.erase(this);
    }
    for (auto& e : _edges) {
        e->_faces.erase(this);
    }
    _material->_faces.erase(this);
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
    _materials.push_back(std::make_shared<MeshMaterial>()); // default material
}

SP<MeshVertex> Mesh::addVertex(glm::vec3 position, glm::vec2 texCoord) {
    auto vertex = std::make_shared<MeshVertex>();
    vertex->setPosition(position);
    vertex->setTexCoord(texCoord);
    _vertices.insert(vertex);
    return vertex;
}

SP<MeshEdge> Mesh::addEdge(const std::array<SP<MeshVertex>, 2> &vertices) {
    auto sortedVertices = vertices;
    std::sort(sortedVertices.begin(), sortedVertices.end());

    auto it = _edges.find(sortedVertices);
    if (it != _edges.end()) {
        return it->second;
    }

    auto edge = std::make_shared<MeshEdge>(vertices);
    _edges[vertices] = edge;
    return edge;
}

SP<MeshFace> Mesh::addFace(const std::vector<SP<MeshVertex> > &vertices, const SP<MeshMaterial>& material) {
    Q_ASSERT(3 <= vertices.size());

    auto sortedVertices = vertices;
    std::sort(sortedVertices.begin(), sortedVertices.end());

    auto it = _faces.find(sortedVertices);
    if (it != _faces.end()) {
        return it->second;
    }

    std::vector<SP<MeshEdge>> edges;
    for (size_t i = 0; i < vertices.size(); ++i) {
        edges.push_back(addEdge({vertices[i], vertices[(i + 1) % vertices.size()]}));
    }

    auto face = std::make_shared<MeshFace>(vertices, edges, material);
    _faces[vertices] = face;
    return face;
}

SP<MeshMaterial> Mesh::addMaterial() {
    auto material = std::make_shared<MeshMaterial>();
    _materials.push_back(material);
    return material;
}

void Mesh::addPlane(dvec3 center, dvec2 size, int normalAxis, const SP<MeshMaterial> &material) {
    std::vector<SP<MeshVertex>> vertices;
    std::array<dvec2, 4> uvs = {dvec2(0, 0), dvec2(1, 0), dvec2(1, 1), dvec2(0, 1)};

    for (dvec2 uv : uvs) {
        dvec3 pos = center;
        pos[(normalAxis + 1) % 3] += size.x * (uv.x - 0.5);
        pos[(normalAxis + 2) % 3] += size.y * (uv.y - 0.5);
        vertices.push_back(addVertex(pos, uv));
    }
    addFace(vertices, material);
}

void Mesh::addCube(glm::dvec3 minPos, glm::dvec3 maxPos, const SP<MeshMaterial> &material) {
    //   2    3
    // 6    7
    //   0    1
    // 4    5

    auto v0 = addVertex(glm::vec3(minPos.x, minPos.y, minPos.z), glm::vec2(0));
    auto v1 = addVertex(glm::vec3(maxPos.x, minPos.y, minPos.z), glm::vec2(0));
    auto v2 = addVertex(glm::vec3(minPos.x, maxPos.y, minPos.z), glm::vec2(0));
    auto v3 = addVertex(glm::vec3(maxPos.x, maxPos.y, minPos.z), glm::vec2(0));
    auto v4 = addVertex(glm::vec3(minPos.x, minPos.y, maxPos.z), glm::vec2(0));
    auto v5 = addVertex(glm::vec3(maxPos.x, minPos.y, maxPos.z), glm::vec2(0));
    auto v6 = addVertex(glm::vec3(minPos.x, maxPos.y, maxPos.z), glm::vec2(0));
    auto v7 = addVertex(glm::vec3(maxPos.x, maxPos.y, maxPos.z), glm::vec2(0));

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
    std::vector<SP<MeshVertex>> vertices;

    double angleStep = M_PI * 2 / segmentCount;
    for (int i = 0 ; i < segmentCount; ++i) {
        double angle = angleStep * i;
        dvec3 offset(0);
        offset[(normalAxis + 1) % 3] = cos(angle);
        offset[(normalAxis + 2) % 3] = sin(angle);
        dvec3 pos = center + offset * radius;
        auto v = addVertex(pos, vec2(0));
        vertices.push_back(v);
    }

    addFace(vertices, material);
}

void Mesh::addSphere(dvec3 center, double radius, int segmentCount, int ringCount, int axis, const SP<MeshMaterial> &material) {
    std::vector<std::vector<SP<MeshVertex>>> vertexMatrix;

    for (int ring = 0; ring < ringCount - 1; ++ring) {
        std::vector<SP<MeshVertex>> vertices;

        double longitude = M_PI * (ring + 1 - ringCount * 0.5) / ringCount;

        for (int i = 0; i < segmentCount; ++i) {
            double latitude = M_PI * 2.0 * (double(i) / segmentCount);
            dvec3 offset;
            offset[axis] = sin(longitude);
            offset[(axis + 1) % 3] = cos(latitude) * cos(longitude);
            offset[(axis + 2) % 3] = sin(latitude) * cos(longitude);
            dvec3 pos = center + offset * radius;
            auto v = addVertex(pos, vec2(0)); // TODO: uv
            vertices.push_back(v);
        }

        vertexMatrix.push_back(std::move(vertices));
    }

    auto bottom = addVertex(center + dvec3(0, -radius, 0), vec2(0));
    auto top = addVertex(center + dvec3(0, radius, 0), vec2(0));

    for (int i = 0; i < segmentCount; ++i) {
        int next = (i + 1) % segmentCount;
        addFace({bottom, vertexMatrix[0][next], vertexMatrix[0][i]}, material);

        for (int ring = 0; ring < ringCount - 2; ++ring) {
            addFace({vertexMatrix[ring][i], vertexMatrix[ring][next], vertexMatrix[ring+1][next], vertexMatrix[ring+1][i]}, material);
        }

        addFace({vertexMatrix[ringCount - 2][i], vertexMatrix[ringCount - 2][next], top}, material);
    }
}

void Mesh::addCone(dvec3 center, double radius, double height, int segmentCount, int axis, const SP<MeshMaterial> &material) {
    std::vector<SP<MeshVertex>> vertices;

    double angleStep = M_PI * 2 / segmentCount;
    for (int i = 0 ; i < segmentCount; ++i) {
        double angle = angleStep * i;
        dvec3 offset(0);
        offset[(axis + 1) % 3] = cos(angle);
        offset[(axis + 2) % 3] = sin(angle);
        dvec3 pos = center + offset * radius;
        auto v = addVertex(pos, vec2(0));
        vertices.push_back(v);
    }

    addFace(vertices, material);

    dvec3 topPosition = center;
    topPosition[axis] += height;

    auto top = addVertex(topPosition, vec2(0));

    for (int i = 0; i < segmentCount; ++i) {
        auto v0 = vertices[i];
        auto v1 = vertices[(i + 1) % segmentCount];
        addFace({v0, v1, top}, material);
    }
}

SP<Mesh> Mesh::clone() const {
    throw std::runtime_error("not implemented");
}

std::vector<SP<MeshFace>> MeshMaterial::faces() const {
    std::vector<SP<MeshFace>> faces;
    faces.reserve(_faces.size());
    for (auto& f : _faces) {
        faces.push_back(f->shared_from_this());
    }
    return faces;
}

} // namespace Lattice
