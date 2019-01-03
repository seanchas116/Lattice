#pragma once
#include <QObject>
#include <glm/glm.hpp>
#include "../support/Pointer.hpp"

namespace Lattice {

class AppState;
class Item;

}

namespace Lattice::Viewport {

class Manipulator;

class ManipulatorController final : public QObject {
    Q_OBJECT
public:
    explicit ManipulatorController(const SP<Manipulator>& manipulator, const SP<AppState>& appState);

    glm::dvec3 position() const;

    void onDragStart();
    void onDrag(glm::dvec3 offset);
    void onDragEnd();

signals:
    void positionChanged(glm::dvec3 position);

private:
    void connectToItem(const SP<Item>& item);

    SP<Manipulator> _manipulator;
    SP<AppState> _appState;
    SP<Item> _item;
    glm::dvec3 _initialPosition;
    QMetaObject::Connection _connection;
};

} // namespace Lattice
