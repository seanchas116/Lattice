#pragma once
#include "../../state/MeshEditState.hpp"
#include "../../mesh/Handle.hpp"
#include "../../document/MeshObject.hpp"
#include "../../viewport/MouseEvent.hpp"
#include "../../viewport/Renderable.hpp"
#include <QObject>
#include <unordered_set>

namespace Lattice {

namespace Mesh {
class Mesh;
}

namespace Editor {
namespace MeshEditor {

class Tool : public Viewport::Renderable {
    Q_OBJECT
public:
    struct EventTarget {
        Opt<Mesh::VertexHandle> vertex;
        Opt<Mesh::EdgeHandle> edge;
        Opt<Mesh::FaceHandle> face;

        std::unordered_set<Mesh::VertexHandle> vertices(const Mesh::Mesh& mesh) const;
        bool operator==(const EventTarget& other) const;
        bool operator!=(const EventTarget& other) const { return !operator==(other); }
    };

    struct HitTestExclusion {
        std::vector<Mesh::VertexHandle> vertices;
        std::vector<Mesh::EdgeHandle> edges;
        std::vector<Mesh::FaceHandle> faces;
    };

    Tool(const SP<State::MeshEditState>& meshEditState) : _meshEditState(meshEditState) {}
    ~Tool();

    auto& meshEditState() const { return _meshEditState; }
    auto& mesh() const { return _meshEditState->mesh(); }
    auto& object() const { return _meshEditState->object(); }

    virtual HitTestExclusion hitTestExclusion() const;

    virtual void mousePressTool(const EventTarget& target, const Viewport::MouseEvent &event) = 0;
    virtual void mouseMoveTool(const EventTarget& target, const Viewport::MouseEvent &event) = 0;
    virtual void mouseReleaseTool(const EventTarget& target, const Viewport::MouseEvent &event) = 0;
    virtual void hoverEnterTool(const EventTarget& target, const Viewport::MouseEvent &event);
    virtual void hoverLeaveTool(const EventTarget& target);
    virtual void keyPressTool(QKeyEvent* event);
    virtual void keyReleaseTool(QKeyEvent* event);

signals:
    void finished();

private:
    SP<State::MeshEditState> _meshEditState;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
