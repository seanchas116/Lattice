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

    //bool isHoverEnabled() const { return _isHoverEnabled; }
    //void setHoverEnabled(bool enabled) { _isHoverEnabled = enabled; }

    virtual void draw(const SP<Operations>& operations, const SP<Camera>& camera);
    virtual void drawPickables(const SP<Operations>& operations, const SP<Camera>& camera);

    virtual Opt<HitResult> hitTest(glm::dvec2 pos, const SP<Camera>& camera) const;

signals:
    void updateRequested();

private:
    //bool _isHoverEnabled = false;
};

} // namespace Renderer
} // namespace Lattice
