#pragma once
#include <QWidget>
#include "../support/OldCamera.hpp"

namespace Lattice {
namespace Editor {

class ViewportControlView : public QWidget {
    Q_OBJECT
public:
    explicit ViewportControlView(const SP<OldCamera>& camera, QWidget *parent = nullptr);

public slots:

private:
    SP<OldCamera> _camera;
};

} // namespace Editor
} // namespace Lattice
