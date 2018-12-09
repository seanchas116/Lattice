#pragma once

#include <QObject>
#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>

namespace Lattice {

class ViewportRenderer final : public QObject, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    ViewportRenderer();

    void resize(glm::ivec2 size);
    void render();

signals:

public slots:
};

} // namespace Lattice
