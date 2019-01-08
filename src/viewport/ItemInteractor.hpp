#pragma once

#include <optional>
#include <glm/glm.hpp>
#include <QObject>
#include "../support/Pointer.hpp"
#include "../support/Location.hpp"
#include "../render/Renderable.hpp"

class QMouseEvent;

namespace Lattice {
class Camera;

namespace Document {
class Item;
}

namespace Viewport {

class ItemPicker;

class ItemInteractor : public QObject, public Render::Renderable {
    Q_OBJECT
public:
    ItemInteractor(const SP<ItemPicker>& picker);

    std::pair<bool, double> mousePress(QMouseEvent* event, glm::dvec2 pos, const Camera& camera) override;
    void mouseMove(QMouseEvent* event, glm::dvec2 pos, const Camera& camera) override;
    void mouseRelease(QMouseEvent* event, glm::dvec2 pos, const Camera& camera) override;

private:
    SP<ItemPicker> _picker;

    std::optional<SP<Document::Item>> _draggedItem;
    bool _dragStarted = false;
    Location _initialLocation;
    glm::dvec3 _initialWorldPos;
    double _initialDragDepth;
};

} // namespace Viewport
} // namespace Lattice
