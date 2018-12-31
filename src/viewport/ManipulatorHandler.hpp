#pragma once
#include <QObject>
#include <glm/glm.hpp>
#include "../support/Pointer.hpp"

namespace Lattice {

class AppState;
class Item;

class ManipulatorHandler final : public QObject {
    Q_OBJECT
public:
    explicit ManipulatorHandler(const SP<AppState>& appState);

    glm::vec3 position() const;

    void onMoveStart();
    void onMove(glm::vec3 offset);
    void onMoveEnd();

signals:
    void positionChanged(glm::vec3 position);

private:
    void connectToItem(const SP<Item>& item);

    SP<AppState> _appState;
    SP<Item> _item;
    QMetaObject::Connection _connection;
};

} // namespace Lattice
