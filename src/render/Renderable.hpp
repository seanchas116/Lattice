#pragma once
#include <QtGlobal>
#include "Operations.hpp"
#include "MouseEvent.hpp"

class QMouseEvent;

namespace Lattice {
namespace Render {

class Renderable {
    Q_DISABLE_COPY(Renderable)
public:
    Renderable();
    virtual ~Renderable();

    virtual void draw(const SP<Operations>& operations, const Camera& camera);

    virtual std::optional<HitResult> hitTest(glm::dvec2 pos, const Camera& camera) const;
    virtual void mousePress(const MouseEvent& event);
    virtual void mouseMove(const MouseEvent& event);
    virtual void mouseRelease(const MouseEvent& event);
    virtual void mouseDoubleClick(const MouseEvent& event);
};

} // namespace Renderer
} // namespace Lattice
