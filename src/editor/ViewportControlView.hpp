#pragma once
#include "../support/Shorthands.hpp"
#include <QWidget>

namespace Lattice {
namespace Editor {

class CameraState;

class ViewportControlView : public QWidget {
    Q_OBJECT
  public:
    explicit ViewportControlView(const SP<CameraState> &cameraState, QWidget *parent = nullptr);

  public slots:

  private:
    SP<CameraState> _cameraState;
};

} // namespace Editor
} // namespace Lattice
