#pragma once
#include <QObject>
#include "../../state/AppState.hpp"
#include "../../mesh/Mesh.hpp"
#include "../../mesh/MeshFragment.hpp"
#include "../../document/MeshItem.hpp"
#include "../../render/MouseEvent.hpp"

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

    Tool(const SP<State::AppState>& appState, const SP<Document::MeshItem>& item) : _appState(appState), _item(item) {}
    ~Tool();

    auto& appState() const { return _appState; }
    auto& item() const { return _item; }

    virtual HitTestExclusion hitTestExclusion() const;

    virtual void mousePress(const EventTarget& target, const Render::MouseEvent &event) = 0;
    virtual void mouseMove(const EventTarget& target, const Render::MouseEvent &event) = 0;
    virtual void mouseRelease(const EventTarget& target, const Render::MouseEvent &event) = 0;
    virtual void hoverEnter(const EventTarget& target, const Render::MouseEvent &event);
    virtual void hoverLeave(const EventTarget& target);
    virtual void keyPress(QKeyEvent* event);
    virtual void keyRelease(QKeyEvent* event);

private:
    SP<State::AppState> _appState;
    SP<Document::MeshItem> _item;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
