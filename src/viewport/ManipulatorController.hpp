#pragma once
#include <QObject>
#include <glm/glm.hpp>
#include "../support/Pointer.hpp"
#include "../support/Location.hpp"

namespace Lattice::UI {
class AppState;
}

namespace Lattice::Document {
class Item;
}

namespace Lattice::Viewport {

class Manipulator;

class ManipulatorController final : public QObject {
    Q_OBJECT
public:
    explicit ManipulatorController(const SP<Manipulator>& manipulator, const SP<UI::AppState>& appState);

    glm::dvec3 position() const;

signals:
    void positionChanged(glm::dvec3 position);

private:
    void onTranslateStarted();
    void onTranslateChanged(int axis, double offset);
    void onTranslateFinished();

    void onScaleStarted();
    void onScaleChanged(int axis, double offset);
    void onScaleFinished();

    void onRotateStarted();
    void onRotateChanged(int axis, double offset);
    void onRotateFinished();

    void connectToItem(const SP<Document::Item>& item);

    SP<Manipulator> _manipulator;
    SP<UI::AppState> _appState;
    SP<Document::Item> _item;
    Location _initialLocation;
    QMetaObject::Connection _connection;
};

} // namespace Lattice
