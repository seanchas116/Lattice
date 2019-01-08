#pragma once
#include <QtGlobal>

namespace Lattice {
namespace Renderer {

class Renderable {
    Q_DISABLE_COPY(Renderable)
public:
    Renderable();
    virtual ~Renderable();
};

} // namespace Renderer
} // namespace Lattice
