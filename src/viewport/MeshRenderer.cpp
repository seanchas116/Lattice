#include "MeshRenderer.hpp"
#include "MeshVAOGenerator.hpp"
#include "Operations.hpp"
#include "../document/Mesh.hpp"
#include "../document/MeshItem.hpp"
#include "../support/Debug.hpp"
#include "../support/Camera.hpp"

using namespace glm;

namespace Lattice::Viewport {

MeshRenderer::MeshRenderer(const SP<Document::MeshItem> &item) : _item(item) {
    // TODO: update mesh when item is changed
    updateVAOs(item->mesh());
    updateBoundingBox();
}

void MeshRenderer::updateVAOs(const SP<Document::Mesh> &mesh) {
    MeshVAOGenerator generator(mesh);
    _vertexVAO = generator.generateVertexVAO();
    _edgeVAO = generator.generateEdgeVAO();
    _faceVAOs= generator.generateFaceVAOs();
}

void MeshRenderer::updateBoundingBox() {
    auto& mesh = _item->mesh();
    vec3 minPos(INFINITY);
    vec3 maxPos(-INFINITY);
    mat4 M = _item->location().matrix();
    for (auto& v : mesh->vertices()) {
        vec3 p = (M * vec4(v->position(), 1)).xyz;
        minPos = min(p, minPos);
        maxPos = max(p, maxPos);
    }

    _boundingBox = Box<float>(minPos, maxPos);
    qDebug() << minPos << maxPos;
}

bool MeshRenderer::intersectsPolygon(const Ray<float> &ray) const {
    // TODO: Use Bounding Volume Hierarchy to do faster
    for (auto& [_, f] : _item->mesh()->faces()) {
        auto v0 = f->vertices()[0]->position();
        for (uint32_t i = 2; i < uint32_t(f->vertices().size()); ++i) {
            auto v1 = f->vertices()[i - 1]->position();
            auto v2 = f->vertices()[i]->position();

            if (ray.intersectsTriangle({v0, v1, v2})) {
                return true;
            }
        }
    }
    return false;
}

void MeshRenderer::drawFaces(const SP<Operations> &operations, const Camera &camera) {
    for (auto& [material, vao] : _faceVAOs) {
        //operations->drawSolid.draw(vao, viewMatrix * _item->location().matrix(), projection, material->baseColor(), vec3(0));
        operations->drawMaterial.draw(vao, _item->location().matrix(), camera, material);
    }
}

void MeshRenderer::drawEdges(const SP<Operations> &operations, const Camera &camera) {
    operations->drawLine.draw(_edgeVAO, _item->location().matrix(), camera, 1.0, dvec3(0));
}

void MeshRenderer::drawVertices(const SP<Operations> &operations, const Camera &camera) {
    operations->drawCircle.draw(_vertexVAO, _item->location().matrix(), camera, 4.0, dvec3(0));
}

bool MeshRenderer::mousePress(QMouseEvent *event, dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);

    auto mouseRay = camera.worldMouseRay(pos);
    if (_boundingBox.intersects(mouseRay)) {
        qDebug() << "clicked:" << _item->name().c_str();

        qDebug() << "mesh intersection" << intersectsPolygon(mouseRay);
        return true;
    }

    return false;
}

bool MeshRenderer::mouseMove(QMouseEvent *event, dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
    // TODO
    return false;
}

bool MeshRenderer::mouseRelease(QMouseEvent *event, dvec2 pos, const Camera &camera) {
    Q_UNUSED(event); Q_UNUSED(pos); Q_UNUSED(camera);
    // TODO
    return false;
}

}
