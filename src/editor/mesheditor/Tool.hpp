#pragma once
#include <QObject>
#include "../../ui/AppState.hpp"
#include "../../mesh/Mesh.hpp"
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
    };

    Tool(const SP<UI::AppState>& appState, const SP<Document::MeshItem>& item) : _appState(appState), _item(item) {}
    ~Tool();

    auto& appState() const { return _appState; }
    auto& item() const { return _item; }

    virtual void mousePress(const EventTarget& target, const Render::MouseEvent &event) = 0;
    virtual void mouseMove(const EventTarget& target, const Render::MouseEvent &event) = 0;
    virtual void mouseRelease(const EventTarget& target, const Render::MouseEvent &event) = 0;
    virtual void hoverEnter(const EventTarget& target, const Render::MouseEvent &event) = 0;
    virtual void hoverLeave(const EventTarget& target) = 0;

private:
    SP<UI::AppState> _appState;
    SP<Document::MeshItem> _item;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
