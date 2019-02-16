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

protected:
    void onUpdate() override;

signals:
    void updated();
};

} // namespace Renderer
} // namespace Lattice
