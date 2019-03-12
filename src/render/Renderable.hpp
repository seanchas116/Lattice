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

    virtual void mousePress(const MouseEvent& event);
    virtual void mouseMove(const MouseEvent& event);
    virtual void mouseRelease(const MouseEvent& event);
    virtual void mouseDoubleClick(const MouseEvent& event);
    virtual void hoverEnter(const MouseEvent& event);
    virtual void hoverLeave();

    void drawRecursive(const SP<Operations>& operations, const SP<Camera>& camera);
    void drawPickablesRecursive(const SP<Operations>& operations, const SP<Camera>& camera, std::vector<SP<Renderable>>& renderedChildren);

    glm::vec4 toIDColor() const;
    static Opt<SP<Renderable>> fromIDColor(glm::vec4 color);

    auto& children() const { return _children; }
    void setChildren(const std::vector<SP<Renderable>>& children);

    void update();

protected:
    virtual void onUpdate();

private:
    Renderable* _parent = nullptr;
    std::vector<SP<Renderable>> _children;
};

} // namespace Render
} // namespace Lattice
