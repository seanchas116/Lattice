#pragma once
#include <QWidget>
#include "../support/Camera.hpp"

namespace Lattice {
namespace Editor {

class ViewportControlView : public QWidget {
    Q_OBJECT
public:
    explicit ViewportControlView(const SP<Camera>& camera, QWidget *parent = nullptr);

public slots:

private:
    SP<Camera> _camera;
};

} // namespace Editor
} // namespace Lattice
