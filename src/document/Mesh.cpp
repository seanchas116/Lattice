#include "Mesh.hpp"
#include "../support/Mod.hpp"

using namespace glm;

namespace Lattice::Document {

glm::vec3 MeshVertex::normal() const {
    glm::vec3 normalSum(0);
    for (auto& face : _faces) {
        normalSum += face->normal();
    }
    return normalize(normalSum / float(_faces.size()));
}

std::vector<SP<MeshFace> > MeshUVPoint::faces() const {
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

class Mesh::AddVertexChange : public Change {
public:
    AddVertexChange(const SP<Mesh>& mesh, const SP<MeshVertex>& vertex) : mesh(mesh), vertex(vertex) {
    }
    void apply() override {
        mesh->_vertices.insert(vertex);
        emit mesh->vertexAdded(vertex);
    }
    SP<Change> invert() const override;

    SP<Mesh> mesh;
    SP<MeshVertex> vertex;
};

class Mesh::RemoveVertexChange : public Change {
public:
    RemoveVertexChange(const SP<Mesh>& mesh, const SP<MeshVertex>& vertex) : mesh(mesh), vertex(vertex) {
    }
    void apply() override {
        mesh->_vertices.erase(vertex);
        emit mesh->vertexRemoved(vertex);
    }
    SP<Change> invert() const override;

    SP<Mesh> mesh;
    SP<MeshVertex> vertex;
};

SP<Change> Mesh::AddVertexChange::invert() const {
    return makeShared<RemoveVertexChange>(mesh, vertex);
}

SP<Change> Mesh::RemoveVertexChange::invert() const {
    return makeShared<AddVertexChange>(mesh, vertex);
}

class Mesh::AddUVPointChange : public Change {
public:
    AddUVPointChange(const SP<Mesh>& mesh, const SP<MeshUVPoint>& uvPoint) : mesh(mesh), uvPoint(uvPoint) {
    }

    void apply() override {
        uvPoint->vertex()->_uvPoints.insert(uvPoint.get());
        emit mesh->uvPointAdded(uvPoint);
    }
    SP<Change> invert() const override;

    SP<Mesh> mesh;
    SP<MeshUVPoint> uvPoint;
};

class Mesh::RemoveUVPointChange : public Change {
public:
    RemoveUVPointChange(const SP<Mesh>& mesh, const SP<MeshUVPoint>& uvPoint) : mesh(mesh), uvPoint(uvPoint) {
    }

    void apply() override {
        uvPoint->vertex()->_uvPoints.erase(uvPoint.get());
        emit mesh->uvPointRemoved(uvPoint);
    }
    SP<Change> invert() const override;

    SP<Mesh> mesh;
    SP<MeshUVPoint> uvPoint;
};

SP<Change> Mesh::AddUVPointChange::invert() const {
    return makeShared<RemoveUVPointChange>(mesh, uvPoint);
}

SP<Change> Mesh::RemoveUVPointChange::invert() const {
    return makeShared<AddUVPointChange>(mesh, uvPoint);
}

class Mesh::AddEdgeChange : public Change {
public:
    AddEdgeChange(const SP<Mesh>& mesh, const SP<MeshEdge>& edge) :
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
    SP<MeshEdge> edge;
};

class Mesh::RemoveEdgeChange : public Change {
public:
    RemoveEdgeChange(const SP<Mesh>& mesh, const SP<MeshEdge>& edge) :
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
    SP<MeshEdge> edge;
};

SP<Change> Mesh::AddEdgeChange::invert() const {
    return makeShared<RemoveEdgeChange>(mesh, edge);
}

SP<Change> Mesh::RemoveEdgeChange::invert() const {
    return makeShared<AddEdgeChange>(mesh, edge);
}


class Mesh::AddFaceChange : public Change {
public:
    AddFaceChange(const SP<Mesh>& mesh, const SP<MeshFace>& face) :
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

        std::set<SP<MeshVertex>> vertexSet(face->vertices().begin(), face->vertices().end());
        mesh->_faces.insert({vertexSet, face});
        emit mesh->faceAdded(face);
    }
    SP<Change> invert() const override;

    SP<Mesh> mesh;
    SP<MeshFace> face;
};

class Mesh::RemoveFaceChange : public Change {
public:
    RemoveFaceChange(const SP<Mesh>& mesh, const SP<MeshFace>& face) :
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
        emit mesh->faceRemoved(face);
    }
    SP<Change> invert() const override;

    SP<Mesh> mesh;
    SP<MeshFace> face;
};

SP<Change> Mesh::AddFaceChange::invert() const {
    return makeShared<RemoveFaceChange>(mesh, face);
}

SP<Change> Mesh::RemoveFaceChange::invert() const {
    return makeShared<AddFaceChange>(mesh, face);
}

class Mesh::SetVertexPositionChange : public Change {
public:
    SetVertexPositionChange(const SP<Mesh>& mesh, const std::unordered_map<SP<MeshVertex>, glm::vec3>& positions) :
        mesh(mesh), newPositions(positions)
    {
    }
    void apply() override {
        std::vector<SP<MeshVertex>> vertices;
        vertices.reserve(newPositions.size());

        for (auto& [v, pos] : newPositions) {
            oldPositions[v] = v->position();
            v->_position = pos;
            vertices.push_back(v);
        }
        emit mesh->verticesChanged(vertices);
    }

    SP<Change> invert() const override {
        return makeShared<SetVertexPositionChange>(mesh, oldPositions);
    }

    bool mergeWith(const SP<const Change>& other) override {
        LATTICE_OPTIONAL_GUARD(change, dynamicPointerCast<const SetVertexPositionChange>(other), return false;)
        if (change->mesh != mesh) { return false; }

        auto oldPositionsMerged = change->oldPositions;
        for (auto& [v, p] : oldPositions) {
            oldPositionsMerged[v] = p;
        }
        oldPositions = oldPositionsMerged;

        for (auto& [v, p] : change->newPositions) {
            newPositions[v] = p;
        }
        return true;
    }

    SP<Mesh> mesh;
    std::unordered_map<SP<MeshVertex>, glm::vec3> oldPositions;
    std::unordered_map<SP<MeshVertex>, glm::vec3> newPositions;
};

class Mesh::SetUVPositionChange : public Change {
public:
    SetUVPositionChange(const SP<Mesh>& mesh, const std::unordered_map<SP<MeshUVPoint>, glm::vec2>& positions) :
        mesh(mesh), newPositions(positions)
    {
    }
    void apply() override {
        std::vector<SP<MeshUVPoint>> uvPoints;
        uvPoints.reserve(newPositions.size());

        for (auto& [v, pos] : newPositions) {
            oldPositions[v] = v->position();
            v->_position = pos;
            uvPoints.push_back(v);
        }
        emit mesh->uvPointsChanged(uvPoints);
    }
    SP<Change> invert() const override {
        return makeShared<SetUVPositionChange>(mesh, oldPositions);
    }
    bool mergeWith(const SP<const Change>& other) override {
        LATTICE_OPTIONAL_GUARD(change, dynamicPointerCast<const SetUVPositionChange>(other), return false;)
        if (change->mesh != mesh) { return false; }

        auto oldPositionsMerged = change->oldPositions;
        for (auto& [v, p] : oldPositions) {
            oldPositionsMerged[v] = p;
        }

        for (auto& [v, p] : change->newPositions) {
            newPositions[v] = p;
        }
        return true;
    }

    SP<Mesh> mesh;
    std::unordered_map<SP<MeshUVPoint>, glm::vec2> oldPositions;
    std::unordered_map<SP<MeshUVPoint>, glm::vec2> newPositions;
};


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
    connect(this, &Mesh::verticesChanged, this, &Mesh::changed);
    connect(this, &Mesh::uvPointsChanged, this, &Mesh::changed);
}

SP<MeshVertex> Mesh::addVertex(glm::vec3 position) {
    auto change = makeShared<AddVertexChange>(sharedFromThis(), makeShared<MeshVertex>());
    _changeHandler(change);
    setPositions({{change->vertex, position}});
    return change->vertex;
}

SP<MeshEdge> Mesh::addEdge(const std::array<SP<MeshVertex>, 2> &vertices) {
    auto it = _edges.find(vertices);
    if (it != _edges.end()) {
        return it->second;
    }
    auto edge = makeShared<MeshEdge>(vertices);
    auto change = makeShared<AddEdgeChange>(sharedFromThis(), edge);
    _changeHandler(change);
    return edge;
}

SP<MeshUVPoint> Mesh::addUVPoint(const SP<MeshVertex> &vertex, vec2 position) {
    auto change = makeShared<AddUVPointChange>(sharedFromThis(), makeShared<MeshUVPoint>(vertex));
    _changeHandler(change);
    setPositions({{change->uvPoint, position}});
    return change->uvPoint;
}

SP<MeshFace> Mesh::addFace(const std::vector<SP<MeshUVPoint> > &uvPoints, const SP<MeshMaterial> &material) {
    Q_ASSERT(3 <= uvPoints.size());

    std::vector<SP<MeshVertex>> vertices;
    for (auto& uv : uvPoints) {
        vertices.push_back((uv->vertex()));
    }

    std::set<SP<MeshVertex>> vertexSet(vertices.begin(), vertices.end());
    auto it = _faces.find(vertexSet);
    if (it != _faces.end()) {
        return it->second;
    }

    std::vector<SP<MeshEdge>> edges;
    for (size_t i = 0; i < vertices.size(); ++i) {
        edges.push_back(addEdge({vertices[i], vertices[(i + 1) % vertices.size()]}));
    }

    auto face = makeShared<MeshFace>(vertices, edges, uvPoints, material);
    auto change = makeShared<AddFaceChange>(sharedFromThis(), face);
    _changeHandler(change);
    return face;
}

SP<MeshMaterial> Mesh::addMaterial() {
    auto material = makeShared<MeshMaterial>();
    _materials.push_back(material);
    return material;
}

void Mesh::setPositions(const std::unordered_map<SP<MeshVertex>, vec3> &positions) {
    auto change = makeShared<SetVertexPositionChange>(sharedFromThis(), positions);
    _changeHandler(change);
}

void Mesh::setPositions(const std::unordered_map<SP<MeshUVPoint>, vec2> &positions) {
    auto change = makeShared<SetUVPositionChange>(sharedFromThis(), positions);
    _changeHandler(change);
}

void Mesh::removeFace(const SP<MeshFace> &face) {
    std::set<SP<MeshVertex>> vertexSet(face->_vertices.begin(), face->_vertices.end());
    auto it = _faces.find(vertexSet);
    if (it == _faces.end()) {
        return;
    }
    auto change = makeShared<RemoveFaceChange>(sharedFromThis(), face);
    _changeHandler(change);
}

void Mesh::removeEdge(const SP<MeshEdge> &edge) {
    auto it = _edges.find(edge->vertices());
    if (it == _edges.end()) {
        return;
    }

    for (auto& face : edge->faces()) {
        removeFace(face);
    }
    auto change = makeShared<RemoveEdgeChange>(sharedFromThis(), edge);
    _changeHandler(change);
}

void Mesh::removeVertex(const SP<MeshVertex> &vertex) {
    for (auto& edge : vertex->edges()) {
        removeEdge(edge->sharedFromThis());
    }
    auto change = makeShared<RemoveVertexChange>(sharedFromThis(), vertex);
    _changeHandler(change);
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

    std::vector<SP<MeshUVPoint>> reverseUVPoints(uvPoints.rbegin(), uvPoints.rend());
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
            otherToNewUVPoints.insert({otherUV->sharedFromThis(), uv});
        }
    }

    for (auto& [_, otherEdge] : other->edges()) {
        Q_UNUSED(_)
        addEdge({otherToNewVertices.at(otherEdge->vertices()[0]), otherToNewVertices.at(otherEdge->vertices()[1])});
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

} // namespace Lattice
