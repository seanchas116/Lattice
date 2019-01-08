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

namespace Lattice::Editor {

class MeshRenderer::FacesRenderer : public Render::Renderable {
public:
    FacesRenderer(MeshRenderer* renderer) : renderer(renderer) {}

    void draw(const SP<Render::Operations>& operations, const Camera& camera) override {
        for (auto& [material, vao] : renderer->_faceVAOs) {
            operations->drawMaterial.draw(vao, renderer->_item->location().matrix(), camera, material);
        }
    }

    MeshRenderer* renderer;
};

class MeshRenderer::EdgesRenderer : public Render::Renderable {
public:
    EdgesRenderer(MeshRenderer* renderer) : renderer(renderer) {}

    void draw(const SP<Render::Operations>& operations, const Camera& camera) override {
        operations->drawLine.draw(renderer->_edgeVAO, renderer->_item->location().matrix(), camera, 1.0, dvec3(0));
    }

    MeshRenderer* renderer;
};

class MeshRenderer::VerticesRenderer : public Render::Renderable {
public:
    VerticesRenderer(MeshRenderer* renderer) : renderer(renderer) {}

    void draw(const SP<Render::Operations>& operations, const Camera& camera) override {
        operations->drawCircle.draw(renderer->_vertexVAO, renderer->_item->location().matrix(), camera, 4.0, dvec3(0));
    }

    MeshRenderer* renderer;
};

MeshRenderer::MeshRenderer(const SP<Document::MeshItem> &item) :
    _item(item),
    _edgeVAO(makeShared<GL::LineVAO>()),
    _vertexVAO(makeShared<GL::PointVAO>()),
    _facesRenderer(makeShared<FacesRenderer>(this)),
    _edgesRenderer(makeShared<EdgesRenderer>(this)),
    _verticesRenderer(makeShared<VerticesRenderer>(this))
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

}
