#pragma once
#include <QtGlobal>
#include <QObject>
#include "../draw/Operations.hpp"
#include "MouseEvent.hpp"
#include "Renderable.hpp"

class QMouseEvent;

namespace Lattice {
namespace Viewport {

class RenderableObject : public QObject, public Renderable {
    Q_OBJECT
public:
    RenderableObject() {}

    bool isVisible() const { return _isVisible; }
    void setVisible(bool visible);

    void drawRecursive(const SP<Draw::Operations>& operations, const SP<Camera>& camera);
    void drawPickablesRecursive(const SP<Draw::Operations>& operations, const SP<Camera>& camera, std::vector<SP<Renderable>>& renderedChildren);
    void draw2DRecursive(QPainter* painter, const QSize& viewportSize);

    auto& childRenderables() const { return _childRenderables; }
    void setChildRenderables(const std::vector<SP<Renderable>>& children);

    void update() { emit updated(); }

signals:
    void updated();

private:
    std::vector<SP<Renderable>> _childRenderables;
    bool _isVisible = true;
};

} // namespace Renderer
} // namespace Lattice
