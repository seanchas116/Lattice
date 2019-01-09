#pragma once
#include "../support/Pointer.hpp"
#include "../support/Location.hpp"
#include <QObject>
#include <glm/glm.hpp>
#include <optional>

namespace Lattice::UI {
class AppState;
}

namespace Lattice::Document {
class Item;
}

namespace Lattice::Editor {

class OldManipulator;

class OldManipulatorController final : public QObject {
    Q_OBJECT
public:
    explicit OldManipulatorController(const SP<OldManipulator>& manipulator, const SP<UI::AppState>& appState);

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

    void connectToItem(const std::optional<SP<Document::Item> > &maybeItem);

    SP<OldManipulator> _manipulator;
    SP<UI::AppState> _appState;
    std::optional<SP<Document::Item>> _item;
    Location _initialLocation;
    QMetaObject::Connection _connection;
};

} // namespace Lattice
