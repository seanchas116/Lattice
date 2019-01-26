#pragma once
#include <QtGlobal>
#include <QObject>
#include "Operations.hpp"
#include "MouseEvent.hpp"

class QMouseEvent;

namespace Lattice {
namespace Render {

class Renderable : public QObject {
    Q_OBJECT
public:
    Renderable();

    virtual void draw(const SP<Operations>& operations, const SP<Camera>& camera);

    virtual std::optional<HitResult> hitTest(glm::dvec2 pos, const SP<Camera>& camera) const;
    virtual void mousePress(const MouseEvent& event);
    virtual void mouseMove(const MouseEvent& event);
    virtual void mouseRelease(const MouseEvent& event);
    virtual void mouseDoubleClick(const MouseEvent& event);

signals:
    void updateRequested();
};

} // namespace Renderer
} // namespace Lattice
