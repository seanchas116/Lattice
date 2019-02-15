#pragma once

#include "MouseEvent.hpp"
#include "../support/SharedPointer.hpp"

namespace Lattice {
namespace Render {

class Renderable : public EnableSharedFromThis<Renderable> {
    Q_DISABLE_COPY(Renderable)
public:
    Renderable() {}
    virtual ~Renderable();

    virtual void mousePress(const MouseEvent& event);
    virtual void mouseMove(const MouseEvent& event);
    virtual void mouseRelease(const MouseEvent& event);
    virtual void mouseDoubleClick(const MouseEvent& event);
    virtual void hoverEnter(const MouseEvent& event);
    virtual void hoverMove(const MouseEvent& event);
    virtual void hoverLeave();

    glm::vec4 toIDColor() const;
    static Opt<SP<Renderable>> fromIDColor(glm::vec4 color);
};

} // namespace Render
} // namespace Lattice
