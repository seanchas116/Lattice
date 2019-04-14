#pragma once
#include <QObject>
#include "../../state/AppState.hpp"
#include "../../mesh/Mesh.hpp"
#include "../../mesh/MeshFragment.hpp"
#include "../../document/MeshObject.hpp"
#include "../../viewport/MouseEvent.hpp"

namespace Lattice {
namespace Editor {
namespace MeshEditor {

class Tool : public QObject {
    Q_OBJECT
public:
    struct EventTarget {
        Opt<SP<Mesh::Vertex>> vertex;
        Opt<SP<Mesh::Edge>> edge;
        Opt<SP<Mesh::Face>> face;

        std::unordered_set<SP<Mesh::Vertex>> vertices() const;
        Mesh::MeshFragment fragment() const;
    };

    struct HitTestExclusion {
        std::vector<SP<Mesh::Vertex>> vertices;
        std::vector<SP<Mesh::Edge>> edges;
        std::vector<SP<Mesh::Face>> faces;
    };

    Tool(const SP<State::AppState>& appState, const SP<Document::MeshObject>& object) : _appState(appState), _object(object) {}
    ~Tool();

    auto& appState() const { return _appState; }
    auto& object() const { return _object; }

    virtual HitTestExclusion hitTestExclusion() const;

    virtual void mousePressEvent(const EventTarget& target, const Viewport::MouseEvent &event) = 0;
    virtual void mouseMoveEvent(const EventTarget& target, const Viewport::MouseEvent &event) = 0;
    virtual void mouseReleaseEvent(const EventTarget& target, const Viewport::MouseEvent &event) = 0;
    virtual void hoverEnterEvent(const EventTarget& target, const Viewport::MouseEvent &event);
    virtual void hoverLeaveEvent(const EventTarget& target);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

    virtual void draw2D(QPainter* painter, const QSize& viewportSize);

private:
    SP<State::AppState> _appState;
    SP<Document::MeshObject> _object;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
