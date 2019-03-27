#pragma once
#include <QtGlobal>
#include <QObject>
#include "Operations.hpp"
#include "MouseEvent.hpp"
#include "Renderable.hpp"

class QMouseEvent;

namespace Lattice {
namespace Render {

class RenderableObject : public QObject, public Renderable {
    Q_OBJECT
public:
    RenderableObject() {}

    void drawRecursive(const SP<Operations>& operations, const SP<Camera>& camera);
    void drawPickablesRecursive(const SP<Operations>& operations, const SP<Camera>& camera, std::vector<SP<Renderable>>& renderedChildren);

    auto& childRenderables() const { return _childRenderables; }
    void setChildRenderables(const std::vector<SP<Renderable>>& children);

    void update() { emit updated(); }

signals:
    void updated();

private:
    std::vector<SP<Renderable>> _childRenderables;
};

} // namespace Renderer
} // namespace Lattice
