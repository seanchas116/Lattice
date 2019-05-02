#pragma once
#include <QObject>
#include "../../state/AppState.hpp"
#include "../../mesh/Handle.hpp"
#include "../../document/MeshObject.hpp"
#include "../../viewport/MouseEvent.hpp"
#include "../../viewport/RenderableObject.hpp"

namespace Lattice {

namespace Mesh {
class Mesh;
}

namespace Editor {
namespace MeshEditor {

class Tool : public Viewport::RenderableObject {
    Q_OBJECT
public:
    struct EventTarget {
        Opt<Mesh::VertexHandle> vertex;
        Opt<Mesh::EdgeHandle> edge;
        Opt<Mesh::FaceHandle> face;

        std::unordered_set<Mesh::VertexHandle> vertices() const;
    };

    struct HitTestExclusion {
        std::vector<Mesh::VertexHandle> vertices;
        std::vector<Mesh::EdgeHandle> edges;
        std::vector<Mesh::FaceHandle> faces;
    };

    Tool(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object, const SP<Mesh::Mesh>& mesh) : _appState(appState),
                                                                                                                    _object(object),
                                                                                                                    _mesh(mesh) {}
    ~Tool();

    auto& appState() const { return _appState; }
    auto& object() const { return _object; }
    auto& mesh() const { return _mesh; }

    virtual HitTestExclusion hitTestExclusion() const;

    virtual void mousePressTool(const EventTarget& target, const Viewport::MouseEvent &event) = 0;
    virtual void mouseMoveTool(const EventTarget& target, const Viewport::MouseEvent &event) = 0;
    virtual void mouseReleaseTool(const EventTarget& target, const Viewport::MouseEvent &event) = 0;
    virtual void hoverEnterTool(const EventTarget& target, const Viewport::MouseEvent &event);
    virtual void hoverLeaveTool(const EventTarget& target);
    virtual void keyPressTool(QKeyEvent* event);
    virtual void keyReleaseTool(QKeyEvent* event);

signals:
    void meshChanged();
    void finished();

private:
    SP<State::AppState> _appState;
    SP<Document::MeshObject> _object;
    SP<Mesh::Mesh> _mesh;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
