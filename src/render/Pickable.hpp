#pragma once

#include "MouseEvent.hpp"

namespace Lattice {
namespace Render {

class Pickable {
    Q_DISABLE_COPY(Pickable)
public:
    Pickable() {}
    virtual ~Pickable();

    virtual void mousePress(const MouseEvent& event);
    virtual void mouseMove(const MouseEvent& event);
    virtual void mouseRelease(const MouseEvent& event);
    virtual void mouseDoubleClick(const MouseEvent& event);
    virtual void hoverEnter(const MouseEvent& event);
    virtual void hoverMove(const MouseEvent& event);
    virtual void hoverLeave();
};

} // namespace Render
} // namespace Lattice
