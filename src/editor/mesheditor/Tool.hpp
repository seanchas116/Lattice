#pragma once
#include <QObject>
#include "../../state/AppState.hpp"
#include "../../mesh/Mesh.hpp"
#include "../../mesh/MeshFragment.hpp"
#include "../../document/MeshObject.hpp"
#include "../../viewport/MouseEvent.hpp"
#include "../../viewport/RenderableObject.hpp"

namespace Lattice {

namespace Editor {
namespace MeshEditor {

class Tool : public Viewport::RenderableObject {
    Q_OBJECT
public:
    struct EventTarget {
        Opt<SP<OldMesh::Vertex>> vertex;
        Opt<SP<OldMesh::Edge>> edge;
        Opt<SP<OldMesh::Face>> face;

        std::unordered_set<SP<OldMesh::Vertex>> vertices() const;
        OldMesh::MeshFragment fragment() const;
    };

    struct HitTestExclusion {
        std::vector<SP<OldMesh::Vertex>> vertices;
        std::vector<SP<OldMesh::Edge>> edges;
        std::vector<SP<OldMesh::Face>> faces;
    };

    Tool(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object) : _appState(appState), _object(object) {}
    ~Tool();

    auto& appState() const { return _appState; }
    auto& object() const { return _object; }

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
    SP<State::AppState> _appState;
    SP<Document::MeshObject> _object;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
