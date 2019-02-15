#pragma once

#include "MouseEvent.hpp"
#include "../support/SharedPointer.hpp"

namespace Lattice {
namespace Render {

class Operations;

class Renderable : public EnableSharedFromThis<Renderable> {
    Q_DISABLE_COPY(Renderable)
public:
    Renderable() {}
    virtual ~Renderable();

    virtual void draw(const SP<Operations>& operations, const SP<Camera>& camera);
    virtual void drawPickables(const SP<Operations>& operations, const SP<Camera>& camera);

    virtual Opt<HitResult> hitTest(glm::dvec2 pos, const SP<Camera>& camera) const;

    virtual void mousePress(const MouseEvent& event);
    virtual void mouseMove(const MouseEvent& event);
    virtual void mouseRelease(const MouseEvent& event);
    virtual void mouseDoubleClick(const MouseEvent& event);
    virtual void hoverEnter(const MouseEvent& event);
    virtual void hoverMove(const MouseEvent& event);
    virtual void hoverLeave();

    glm::vec4 toIDColor() const;
    static Opt<SP<Renderable>> fromIDColor(glm::vec4 color);

    auto& children() const { return _children; }
    void setChildren(const std::vector<SP<Renderable>>& children);

    Opt<SP<Renderable>> parent() const;

private:
    std::vector<SP<Renderable>> _children;
    WP<Renderable> _parent;
};

} // namespace Render
} // namespace Lattice
