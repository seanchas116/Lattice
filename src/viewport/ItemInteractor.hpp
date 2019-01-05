#pragma once

#include <glm/glm.hpp>
#include <QObject>
#include "../support/Pointer.hpp"
#include "../support/Location.hpp"

class QMouseEvent;

namespace Lattice {
class Camera;

namespace Document {
class Item;
}

namespace Viewport {

class ItemPicker;

class ItemInteractor : public QObject {
    Q_OBJECT
public:
    ItemInteractor(const SP<ItemPicker>& picker);

    bool mousePress(QMouseEvent* event, glm::dvec2 pos, const Camera& camera);
    bool mouseMove(QMouseEvent* event, glm::dvec2 pos, const Camera& camera);
    bool mouseRelease(QMouseEvent* event, glm::dvec2 pos, const Camera& camera);

private:
    SP<ItemPicker> _picker;

    SP<Document::Item> _draggedItem;
    Location _initialLocation;
    glm::dvec3 _initialWorldPos;
    double _initialDragDepth;
};

} // namespace Viewport
} // namespace Lattice
