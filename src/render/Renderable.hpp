#pragma once
#include <QtGlobal>
#include "Operations.hpp"

class QMouseEvent;

namespace Lattice {
namespace Render {

class Renderable {
    Q_DISABLE_COPY(Renderable)
public:
    Renderable();
    virtual ~Renderable();

    virtual void draw(const SP<Operations>& operations, const Camera& camera);

    // The Renderable which returned true in first value and largest t in second value is considered dragged
    virtual std::pair<bool, double> mousePress(QMouseEvent* event, glm::dvec2 pos, const Camera& camera);
    virtual void mouseMove(QMouseEvent* event, glm::dvec2 pos, const Camera& camera);
    virtual void mouseRelease(QMouseEvent* event, glm::dvec2 pos, const Camera& camera);

    auto& children() const { return _children; }
    void setChildren(const std::vector<SP<Renderable>> &children) { _children = children; }

private:
    std::vector<SP<Renderable>> _children;
};

} // namespace Renderer
} // namespace Lattice
