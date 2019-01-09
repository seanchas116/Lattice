#pragma once
#include <QObject>
#include "../../support/Location.hpp"
#include "../../render/Renderable.hpp"

namespace Lattice {

namespace UI {
class AppState;
}
namespace Document {
class Item;
}

namespace Editor {
namespace Manipulator {

class ManipulatorController : public QObject {
    Q_OBJECT
public:
    ManipulatorController(const SP<UI::AppState>& appState);

    glm::dvec3 position() const;

    auto& translateHandles() const { return _translateHandles; }

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

    void connectToItem(const std::optional<SP<Document::Item>> &maybeItem);

    SP<UI::AppState> _appState;
    std::vector<SP<Render::Renderable>> _translateHandles;
    std::optional<SP<Document::Item>> _item;
    Location _initialLocation;
    QMetaObject::Connection _connection;
};

}
} // namespace Editor
} // namespace Lattice
