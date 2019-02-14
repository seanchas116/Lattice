#pragma once

#include "MouseEvent.hpp"
#include "../support/SharedPointer.hpp"

namespace Lattice {
namespace Render {

class Pickable : public EnableSharedFromThis<Pickable> {
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

    glm::vec4 toIDColor() const;
    static Opt<SP<Pickable>> fromIDColor(glm::vec4 color);
};

} // namespace Render
} // namespace Lattice
