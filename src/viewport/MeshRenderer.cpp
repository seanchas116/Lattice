#include "MeshRenderer.hpp"
#include "../document/MeshShape.hpp"
#include "../gl/VAO.hpp"
#include "../gl/LineVAO.hpp"
#include "../gl/VertexBuffer.hpp"

using namespace glm;

namespace Lattice {

MeshRenderer::MeshRenderer() {
    _faceVAO = std::make_shared<VAO>();
    _edgeVAO = std::make_shared<LineVAO>();
}

void MeshRenderer::update(const SP<MeshShape> &shape) {
    {
        // Edge VAO
        std::unordered_map<SP<Vertex>, int> indices;
        std::vector<VertexBuffer::Vertex> vertices;
        for (auto& vertex : shape->vertices()) {
            indices[vertex] = vertices.size();
            VertexBuffer::Vertex vertexData = {
                    vertex->position(),
                    vec2(0), // TODO
                    vec3(0) // TODO
            };
            vertices.push_back(vertexData);
        }
        // WIP
    }
}

}
