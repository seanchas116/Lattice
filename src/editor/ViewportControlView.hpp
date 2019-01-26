#pragma once
#include <QWidget>
#include "../support/Camera.hpp"

namespace Lattice {
namespace Editor {

class ViewportControlView : public QWidget {
    Q_OBJECT
public:
    explicit ViewportControlView(QWidget *parent = nullptr);

    auto cameraProjection() const { return _projection; }
    void setCameraProjection(Camera::Projection projection);

signals:
    void cameraProjectionChanged(Camera::Projection projection);

public slots:

private:
    Camera::Projection _projection = Camera::Projection::Perspective;
};

} // namespace Editor
} // namespace Lattice
