#include "MeshRenderer.hpp"
#include "MeshVAOGenerator.hpp"
#include "Operations.hpp"
#include "../document/Mesh.hpp"
#include "../document/MeshItem.hpp"

using namespace glm;

namespace Lattice::Viewport {

MeshRenderer::MeshRenderer(const SP<Document::MeshItem> &item) : _item(item) {
    // TODO: update mesh when item is changed
    update(item->mesh());
}

void MeshRenderer::update(const SP<Document::Mesh> &mesh) {
    MeshVAOGenerator generator(mesh);
    _vertexVAO = generator.generateVertexVAO();
    _edgeVAO = generator.generateEdgeVAO();
    _faceVAOs= generator.generateFaceVAOs();
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

}
