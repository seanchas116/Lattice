#include "Mesh.hpp"
#include "../support/Mod.hpp"
#include "../support/Debug.hpp"
#include "../support/PropertyChange.hpp"

using namespace glm;

namespace Lattice::Mesh {

SP<UVPoint> Vertex::firstUVPoint() const {
    return (*_uvPoints.begin())->sharedFromThis();
}

glm::vec3 Vertex::normal() const {
    glm::vec3 normalSum(0);
    for (auto& face : _faces) {
        normalSum += face->normal();
    }
    return normalize(normalSum / float(_faces.size()));
}

std::vector<SP<Face> > UVPoint::faces() const {
    std::vector<SP<Face>> faces;
    for (auto& f : _faces) {
        faces.push_back(f->sharedFromThis());
    }
    return faces;
}

std::vector<SP<Face> > Edge::faces() const {
    std::vector<SP<Face>> faces;
    for (auto& f : _faces) {
        faces.push_back(f->sharedFromThis());
    }
    return faces;
}

glm::vec3 Face::normal() const {
    if (_vertices.size() == 3) {
        return normalize(cross(_vertices[1]->position() - _vertices[0]->position(), _vertices[2]->position() - _vertices[0]->position()));
    }

    // find average vertex normal
    glm::vec3 normalSum(0);
    int sumCount = 0;
    int vertexCount = int(_vertices.size());

    for (int i = 0; i < vertexCount; ++i) {
        auto prev = _vertices[i]->position();
        auto curr = _vertices[(i + 1) % vertexCount]->position();
        auto next = _vertices[(i + 2) % vertexCount]->position();
        auto crossValue = cross(next- curr, prev - curr);
        if (crossValue == vec3(0)) {
            continue;
        }
        auto normal = normalize(crossValue);
        normalSum += normal;
        ++sumCount;
    }
    if (sumCount == 0) {
        return vec3(0); // TODO: what should we do?
    }
    return normalize(normalSum);
}

class Mesh::AddVertexChange : public Change {
public:
    AddVertexChange(const SP<Mesh>& mesh, const SP<Vertex>& vertex) : mesh(mesh), vertex(vertex) {
    }
    void apply() override {
        mesh->_vertices.insert(vertex);
        emit mesh->vertexAdded(vertex);
    }
    SP<Change> invert() const override;

    SP<Mesh> mesh;
    SP<Vertex> vertex;
};

class Mesh::RemoveVertexChange : public Change {
public:
    RemoveVertexChange(const SP<Mesh>& mesh, const SP<Vertex>& vertex) : mesh(mesh), vertex(vertex) {
    }
    void apply() override {
        mesh->_vertices.erase(vertex);
        emit mesh->vertexRemoved(vertex);
    }
    SP<Change> invert() const override;

    SP<Mesh> mesh;
    SP<Vertex> vertex;
};

SP<Change> Mesh::AddVertexChange::invert() const {
    return makeShared<RemoveVertexChange>(mesh, vertex);
}

SP<Change> Mesh::RemoveVertexChange::invert() const {
    return makeShared<AddVertexChange>(mesh, vertex);
}

class Mesh::AddUVPointChange : public Change {
public:
    AddUVPointChange(const SP<Mesh>& mesh, const SP<UVPoint>& uvPoint) : mesh(mesh), uvPoint(uvPoint) {
    }

    void apply() override {
        uvPoint->vertex()->_uvPoints.insert(uvPoint.get());
        emit mesh->uvPointAdded(uvPoint);
    }
    SP<Change> invert() const override;

    SP<Mesh> mesh;
    SP<UVPoint> uvPoint;
};

class Mesh::RemoveUVPointChange : public Change {
public:
    RemoveUVPointChange(const SP<Mesh>& mesh, const SP<UVPoint>& uvPoint) : mesh(mesh), uvPoint(uvPoint) {
    }

    void apply() override {
        uvPoint->vertex()->_uvPoints.erase(uvPoint.get());
        emit mesh->uvPointRemoved(uvPoint);
    }
    SP<Change> invert() const override;

    SP<Mesh> mesh;
    SP<UVPoint> uvPoint;
};

SP<Change> Mesh::AddUVPointChange::invert() const {
    return makeShared<RemoveUVPointChange>(mesh, uvPoint);
}

SP<Change> Mesh::RemoveUVPointChange::invert() const {
    return makeShared<AddUVPointChange>(mesh, uvPoint);
}

class Mesh::AddEdgeChange : public Change {
public:
    AddEdgeChange(const SP<Mesh>& mesh, const SP<Edge>& edge) :
        mesh(mesh),
        edge(edge)
    {
    }

    void apply() override {
        edge->_vertices[0]->_edges.insert(edge.get());
        edge->_vertices[1]->_edges.insert(edge.get());
        mesh->_edges.insert({edge->_vertices, edge});
        emit mesh->edgeAdded(edge);
    }
    SP<Change> invert() const override;

    SP<Mesh> mesh;
    SP<Edge> edge;
};

class Mesh::RemoveEdgeChange : public Change {
public:
    RemoveEdgeChange(const SP<Mesh>& mesh, const SP<Edge>& edge) :
        mesh(mesh),
        edge(edge)
    {
    }

    void apply() override {
        edge->_vertices[0]->_edges.erase(edge.get());
        edge->_vertices[1]->_edges.erase(edge.get());
        mesh->_edges.erase(edge->_vertices);
        emit mesh->edgeRemoved(edge);
    }
    SP<Change> invert() const override;

    SP<Mesh> mesh;
    SP<Edge> edge;
};

SP<Change> Mesh::AddEdgeChange::invert() const {
    return makeShared<RemoveEdgeChange>(mesh, edge);
}

SP<Change> Mesh::RemoveEdgeChange::invert() const {
    return makeShared<AddEdgeChange>(mesh, edge);
}


class Mesh::AddFaceChange : public Change {
public:
    AddFaceChange(const SP<Mesh>& mesh, const SP<Face>& face) :
        mesh(mesh),
        face(face)
    {
    }

    void apply() override {
        for (auto& v : face->vertices()) {
            v->_faces.insert(face.get());
        }
        for (auto& e : face->edges()) {
            e->_faces.insert(face.get());
        }
        for (auto& uv : face->uvPoints()) {
            uv->_faces.insert(face.get());
        }
        face->material()->_faces.insert(face.get());

        std::set<SP<Vertex>> vertexSet(face->vertices().begin(), face->vertices().end());
        mesh->_faces.insert({vertexSet, face});
        emit mesh->faceAdded(face);
    }
    SP<Change> invert() const override;

    SP<Mesh> mesh;
    SP<Face> face;
};

class Mesh::RemoveFaceChange : public Change {
public:
    RemoveFaceChange(const SP<Mesh>& mesh, const SP<Face>& face) :
        mesh(mesh),
        face(face)
    {
    }

    void apply() override {
        for (auto& v : face->_vertices) {
            v->_faces.erase(face.get());
        }
        for (auto& e : face->_edges) {
            e->_faces.erase(face.get());
        }
        for (auto& uv : face->_uvPoints) {
            uv->_faces.erase(face.get());
        }
        face->_material->_faces.erase(face.get());

        std::set<SP<Vertex>> vertexSet(face->vertices().begin(), face->vertices().end());
        mesh->_faces.erase(vertexSet);
        emit mesh->faceRemoved(face);
    }
    SP<Change> invert() const override;

    SP<Mesh> mesh;
    SP<Face> face;
};

SP<Change> Mesh::AddFaceChange::invert() const {
    return makeShared<RemoveFaceChange>(mesh, face);
}

SP<Change> Mesh::RemoveFaceChange::invert() const {
    return makeShared<AddFaceChange>(mesh, face);
}

Mesh::Mesh() {
    _changeHandler = [](const auto& change) {
        change->apply();
    };

    connect(this, &Mesh::vertexAdded, this, &Mesh::topologyChanged);
    connect(this, &Mesh::vertexRemoved, this, &Mesh::topologyChanged);
    connect(this, &Mesh::uvPointAdded, this, &Mesh::topologyChanged);
    connect(this, &Mesh::uvPointRemoved, this, &Mesh::topologyChanged);
    connect(this, &Mesh::edgeAdded, this, &Mesh::topologyChanged);
    connect(this, &Mesh::edgeRemoved, this, &Mesh::topologyChanged);
    connect(this, &Mesh::faceAdded, this, &Mesh::topologyChanged);
    connect(this, &Mesh::faceRemoved, this, &Mesh::topologyChanged);

    connect(this, &Mesh::topologyChanged, this, &Mesh::changed);
    connect(this, &Mesh::vertexChanged, this, &Mesh::changed);
    connect(this, &Mesh::uvPointChanged, this, &Mesh::changed);
}

SP<Vertex> Mesh::addVertex(glm::vec3 position) {
    auto change = makeShared<AddVertexChange>(sharedFromThis(), makeShared<Vertex>());
    _changeHandler(change);
    setPositions({{change->vertex, position}});
    return change->vertex;
}

SP<Edge> Mesh::addEdge(const SortedArray<SP<Vertex>, 2> &vertices) {
    auto it = _edges.find(vertices);
    if (it != _edges.end()) {
        return it->second;
    }
    auto edge = makeShared<Edge>(vertices);
    auto change = makeShared<AddEdgeChange>(sharedFromThis(), edge);
    _changeHandler(change);

    std::vector<SP<Face>> facesToRemove;
    std::vector<std::tuple<std::vector<SP<UVPoint>>, std::vector<SP<UVPoint>>, SP<Material>>> faceAdditions;

    // cut faces that includes newly added edge
    for (auto& [_, face] : _faces) {
        auto& faceUVPoints = face->uvPoints();
        auto uv0It = std::find_if(faceUVPoints.begin(), faceUVPoints.end(), [&](auto& uv) { return uv->vertex() == vertices[0]; });
        auto uv1It = std::find_if(faceUVPoints.begin(), faceUVPoints.end(), [&](auto& uv) { return uv->vertex() == vertices[1]; });
        if (uv0It != faceUVPoints.end() && uv1It != faceUVPoints.end()) {
            // face includes edge
            if (uv1It < uv0It) {
                std::swap(uv0It, uv1It);
            }

            std::vector<SP<UVPoint>> uvPoints0;
            uvPoints0.insert(uvPoints0.end(), uv1It, faceUVPoints.end());
            uvPoints0.insert(uvPoints0.end(), faceUVPoints.begin(), uv0It + 1);
            std::vector<SP<UVPoint>> uvPoints1(uv0It, uv1It + 1);

            faceAdditions.push_back({uvPoints0, uvPoints1, face->material()});
            facesToRemove.push_back(face);
        }
    }
    for (auto& [uvPoints0, uvPoints1, material] : faceAdditions) {
        addFace(uvPoints0, material);
        addFace(uvPoints1, material);
    }
    for (auto& f : facesToRemove) {
        removeFace(f);
    }

    return edge;
}

SP<UVPoint> Mesh::addUVPoint(const SP<Vertex> &vertex, vec2 position) {
    auto change = makeShared<AddUVPointChange>(sharedFromThis(), makeShared<UVPoint>(vertex));
    _changeHandler(change);
    setPositions({{change->uvPoint, position}});
    return change->uvPoint;
}

SP<Face> Mesh::addFace(const std::vector<SP<UVPoint> > &uvPoints, const SP<Material> &material) {
    Q_ASSERT(3 <= uvPoints.size());

    std::vector<SP<Vertex>> vertices;
    for (auto& uv : uvPoints) {
        vertices.push_back((uv->vertex()));
    }

    std::set<SP<Vertex>> vertexSet(vertices.begin(), vertices.end());
    auto it = _faces.find(vertexSet);
    if (it != _faces.end()) {
        return it->second;
    }

    std::vector<SP<Edge>> edges;
    for (size_t i = 0; i < vertices.size(); ++i) {
        edges.push_back(addEdge({vertices[i], vertices[(i + 1) % vertices.size()]}));
    }

    auto face = makeShared<Face>(vertices, edges, uvPoints, material);
    auto change = makeShared<AddFaceChange>(sharedFromThis(), face);
    _changeHandler(change);
    return face;
}

SP<Material> Mesh::addMaterial() {
    auto material = makeShared<Material>();
    _materials.push_back(material);
    return material;
}

void Mesh::setPositions(const std::unordered_map<SP<Vertex>, vec3> &positions) {
    auto setter = [self = sharedFromThis()](const SP<Vertex>& vertex, vec3 pos) {
        vertex->_position = pos;
        emit self->vertexChanged(vertex);
    };
    auto getter = [](const SP<Vertex>& vertex) {
        return vertex->_position;
    };

    auto change = makeShared<PropertyChange<Vertex, vec3>>(positions, getter, setter);
    _changeHandler(change);
}

void Mesh::setPositions(const std::unordered_map<SP<UVPoint>, vec2> &positions) {
    auto setter = [mesh = sharedFromThis()](const SP<UVPoint>& uvPoint, vec2 pos) {
        uvPoint->_position = pos;
        emit mesh->uvPointChanged(uvPoint);
    };
    auto getter = [](const SP<UVPoint>& uvPoint) {
        return uvPoint->_position;
    };

    auto change = makeShared<PropertyChange<UVPoint, vec2>>(positions, getter, setter);
    _changeHandler(change);
}

void Mesh::removeFace(const SP<Face> &face) {
    std::set<SP<Vertex>> vertexSet(face->_vertices.begin(), face->_vertices.end());
    auto it = _faces.find(vertexSet);
    if (it == _faces.end()) {
        return;
    }
    auto change = makeShared<RemoveFaceChange>(sharedFromThis(), face);
    _changeHandler(change);
}

void Mesh::removeEdge(const SP<Edge> &edge) {
    auto it = _edges.find(edge->vertices());
    if (it == _edges.end()) {
        return;
    }

    auto faces = edge->faces();
    for (auto& face : faces) {
        removeFace(face);
    }
    auto change = makeShared<RemoveEdgeChange>(sharedFromThis(), edge);
    _changeHandler(change);
}

void Mesh::removeVertex(const SP<Vertex> &vertex) {
    auto edges = vertex->edges();
    for (auto& edge : edges) {
        removeEdge(edge->sharedFromThis());
    }
    auto change = makeShared<RemoveVertexChange>(sharedFromThis(), vertex);
    _changeHandler(change);
}

void Mesh::addPlane(dvec3 center, dvec2 size, int normalAxis, const SP<Material> &material) {
    std::vector<SP<UVPoint>> uvPoints;
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

void Mesh::addCube(glm::dvec3 minPos, glm::dvec3 maxPos, const SP<Material> &material) {
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

void Mesh::addCircle(glm::dvec3 center, double radius, int segmentCount, Mesh::CircleFill fill, int normalAxis, const SP<Material> &material) {
    Q_UNUSED(fill);
    // TODO: use CircleFill
    std::vector<SP<UVPoint>> uvPoints;

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

void Mesh::addSphere(dvec3 center, double radius, int segmentCount, int ringCount, int axis, const SP<Material> &material) {
    std::vector<std::vector<SP<UVPoint>>> uvPointMatrix;

    for (int ring = 0; ring < ringCount - 1; ++ring) {
        std::vector<SP<UVPoint>> uvPoints;

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

void Mesh::addCone(dvec3 center, double radius, double height, int segmentCount, int axis, const SP<Material> &material) {
    std::vector<SP<UVPoint>> uvPoints;

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

    std::vector<SP<UVPoint>> reverseUVPoints(uvPoints.rbegin(), uvPoints.rend());
    addFace(reverseUVPoints, material);

    dvec3 topPosition = center;
    topPosition[axis] += height;

    auto top = addUVPoint(addVertex(topPosition), vec2(0));

    for (int i = 0; i < segmentCount; ++i) {
        auto v0 = uvPoints[i];
        auto v1 = uvPoints[(i + 1) % segmentCount];
        addFace({v0, v1, top}, material);
    }
}

void Mesh::addCylinder(dvec3 center, double radius, double height, int segmentCount, int axis, const SP<Material> &material) {
    double angleStep = M_PI * 2 / segmentCount;

    std::vector<SP<UVPoint>> bottomUVPoints;
    bottomUVPoints.reserve(segmentCount);

    for (int i = 0 ; i < segmentCount; ++i) {
        double angle = angleStep * i;
        dvec3 offset(0);
        offset[(axis + 1) % 3] = cos(angle) * radius;
        offset[(axis + 2) % 3] = sin(angle) * radius;
        dvec3 pos = center + offset;
        auto v = addUVPoint(addVertex(pos), vec2(0));
        bottomUVPoints.push_back(v);
    }

    std::vector<SP<UVPoint>> reverseBottomUVPoints(bottomUVPoints.rbegin(), bottomUVPoints.rend());
    addFace(reverseBottomUVPoints, material);

    std::vector<SP<UVPoint>> topUVPoints;
    topUVPoints.reserve(segmentCount);

    for (int i = 0 ; i < segmentCount; ++i) {
        double angle = angleStep * i;
        dvec3 offset(0);
        offset[(axis + 1) % 3] = cos(angle) * radius;
        offset[(axis + 2) % 3] = sin(angle) * radius;
        offset[axis] = height;
        dvec3 pos = center + offset;
        auto v = addUVPoint(addVertex(pos), vec2(0));
        topUVPoints.push_back(v);
    }

    addFace(topUVPoints, material);

    for (int i = 0; i < segmentCount; ++i) {
        auto v0 = bottomUVPoints[i];
        auto v1 = bottomUVPoints[(i + 1) % segmentCount];
        auto v2 = topUVPoints[(i + 1) % segmentCount];
        auto v3 = topUVPoints[i];
        addFace({v0, v1, v2, v3}, material);
    }
}

void Mesh::merge(const SP<const Mesh> &other) {
    std::unordered_map<SP<Material>, SP<Material>> otherToNewMaterials;
    std::unordered_map<SP<Vertex>, SP<Vertex>> otherToNewVertices;
    std::unordered_map<SP<UVPoint>, SP<UVPoint>> otherToNewUVPoints;

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
            otherToNewUVPoints.insert({otherUV->sharedFromThis(), uv});
        }
    }

    for (auto& [_, otherEdge] : other->edges()) {
        Q_UNUSED(_)
        addEdge({otherToNewVertices.at(otherEdge->vertices()[0]), otherToNewVertices.at(otherEdge->vertices()[1])});
    }
    for (auto& [_, otherFace] : other->faces()) {
        Q_UNUSED(_)
        std::vector<SP<UVPoint>> uvPoints;
        for (auto& otherUV : otherFace->uvPoints()) {
            uvPoints.push_back(otherToNewUVPoints.at(otherUV));
        }
        addFace(uvPoints, otherToNewMaterials.at(otherFace->material()));
    }
}

SP<Face> Mesh::flipFace(const SP<Face> &face) {
    auto uvPoints = face->uvPoints();
    auto material = face->material();
    std::vector<SP<UVPoint>> reverseUVPoints(uvPoints.rbegin(), uvPoints.rend());

    removeFace(face);
    return addFace(reverseUVPoints, material);
}

SP<Vertex> Mesh::cutEdge(const SP<Edge> &edge, float t) {
    auto pos = edge->ray().at(t);

    auto uv = addUVPoint(addVertex(pos), vec2(0)); // Use better UV position

    auto edge1 = addEdge({edge->vertices()[0], uv->vertex()});
    auto edge2 = addEdge({uv->vertex(), edge->vertices()[1]});

    auto faces = edge->faces();
    for (auto& face : faces) {
        std::vector<SP<UVPoint>> newFaceUVPoints;
        for (size_t i = 0; i < face->uvPoints().size(); ++i) {
            auto uv0 = face->uvPoints()[i];
            auto uv1 = face->uvPoints()[(i + 1) % face->uvPoints().size()];

            newFaceUVPoints.push_back(uv0);

            // TODO: create separate uvpoint if possible when uv is split at the edge
            if (uv0->vertex() == edge->vertices()[0] && uv1->vertex() == edge->vertices()[1]) {
                newFaceUVPoints.push_back(uv);
            } else if (uv1->vertex() == edge->vertices()[0] && uv0->vertex() == edge->vertices()[1]) {
                newFaceUVPoints.push_back(uv);
            }
        }

        addFace(newFaceUVPoints, face->material());
    }
    for (auto& face : faces) {
        removeFace(face);
    }
    removeEdge(edge);

    return uv->vertex();
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

} // namespace Lattice
