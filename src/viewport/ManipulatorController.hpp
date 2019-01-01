#pragma once
#include <QObject>
#include <glm/glm.hpp>
#include "../support/Pointer.hpp"

namespace Lattice {

class AppState;
class Item;

class ManipulatorController final : public QObject {
    Q_OBJECT
public:
    explicit ManipulatorController(const SP<AppState>& appState);

    glm::vec3 position() const;

    void onDragStart();
    void onDrag(glm::vec3 offset);
    void onDragEnd();

signals:
    void positionChanged(glm::vec3 position);

private:
    void connectToItem(const SP<Item>& item);

    SP<AppState> _appState;
    SP<Item> _item;
    glm::vec3 _initialPosition;
    QMetaObject::Connection _connection;
};

} // namespace Lattice
