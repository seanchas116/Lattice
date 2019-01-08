#include "MeshRenderer.hpp"
#include "MeshVAOGenerator.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/PointVAO.hpp"
#include "../document/Document.hpp"
#include "../document/Mesh.hpp"
#include "../document/MeshItem.hpp"
#include "../support/Debug.hpp"
#include "../support/Camera.hpp"

using namespace glm;

namespace Lattice::Viewport {

MeshRenderer::MeshRenderer(const SP<Document::MeshItem> &item) :
    _item(item),
    _edgeVAO(makeShared<GL::LineVAO>()),
    _vertexVAO(makeShared<GL::PointVAO>())
{
    // TODO: update mesh when item is changed
    updateVAOs(item->mesh());
}

void MeshRenderer::updateVAOs(const SP<Document::Mesh> &mesh) {
    MeshVAOGenerator generator(mesh);
    _vertexVAO = generator.generateVertexVAO();
    _edgeVAO = generator.generateEdgeVAO();
    _faceVAOs= generator.generateFaceVAOs();
}

void MeshRenderer::drawFaces(const SP<Render::Operations> &operations, const Camera &camera) {
    for (auto& [material, vao] : _faceVAOs) {
        //operations->drawSolid.draw(vao, viewMatrix * _item->location().matrix(), projection, material->baseColor(), vec3(0));
        operations->drawMaterial.draw(vao, _item->location().matrix(), camera, material);
    }
}

void MeshRenderer::drawEdges(const SP<Render::Operations> &operations, const Camera &camera) {
    operations->drawLine.draw(_edgeVAO, _item->location().matrix(), camera, 1.0, dvec3(0));
}

void MeshRenderer::drawVertices(const SP<Render::Operations> &operations, const Camera &camera) {
    operations->drawCircle.draw(_vertexVAO, _item->location().matrix(), camera, 4.0, dvec3(0));
}

}
