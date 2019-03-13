#pragma once
#include <QObject>
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
    ~Tool();

    virtual void mousePress(const EventTarget& target, const Render::MouseEvent &event) = 0;
    virtual void mouseMove(const EventTarget& target, const Render::MouseEvent &event) = 0;
    virtual void mouseRelease(const EventTarget& target, const Render::MouseEvent &event) = 0;
    virtual void hoverEnter(const EventTarget& target, const Render::MouseEvent &event) = 0;
    virtual void hoverLeave(const EventTarget& target) = 0;
};

} // namespace MeshEditor
} // namespace Editor
} // namespace Lattice
