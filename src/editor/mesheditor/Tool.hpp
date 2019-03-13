#pragma once
#include <QObject>
#include "../../ui/AppState.hpp"
#include "../../mesh/Mesh.hpp"
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
    };

    Tool(const SP<UI::AppState>& appState) : _appState(appState) {}
    ~Tool();

    auto& appState() const { return _appState; }

    virtual void mousePress(const EventTarget& target, const Render::MouseEvent &event) = 0;
    virtual void mouseMove(const EventTarget& target, const Render::MouseEvent &event) = 0;
    virtual void mouseRelease(const EventTarget& target, const Render::MouseEvent &event) = 0;
    virtual void hoverEnter(const EventTarget& target, const Render::MouseEvent &event) = 0;
    virtual void hoverLeave(const EventTarget& target) = 0;

private:
    SP<UI::AppState> _appState;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
