#pragma once
#include <QObject>
#include "../render/RenderWidget.hpp"

namespace Lattice {
namespace Editor {

class Background;
class GridFloor;

class EditorScene : public QObject {
    Q_OBJECT
public:
    EditorScene();

    auto& layers() const { return _layers; }

    void updateLayers();

signals:
    void updateRequested();

private:
    std::vector<Render::Layer> _layers;
    SP<Background> _background;
    SP<GridFloor> _gridFloor;
};

} // namespace Editor
} // namespace Lattice
