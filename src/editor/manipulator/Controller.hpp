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

class Controller : public QObject {
    Q_OBJECT
public:
    Controller(const SP<UI::AppState>& appState);

    glm::dvec3 position() const;

    auto& translateHandles() const { return _translateHandles; }
    auto& scaleHandles() const { return _scaleHandles; }
    auto& rotateHandles() const { return _rotateHandles; }

signals:
    void positionChanged(glm::dvec3 position);

private:
    enum class ValueType {
        Translate, Scale, Rotate
    };

    void onBegin(ValueType type, double value);
    void onChange(ValueType type, int axis, double value);
    void onEnd(ValueType type);

    void connectToItem(const std::optional<SP<Document::Item>> &maybeItem);

    SP<UI::AppState> _appState;
    std::vector<SP<Render::Renderable>> _translateHandles;
    std::vector<SP<Render::Renderable>> _scaleHandles;
    std::vector<SP<Render::Renderable>> _rotateHandles;
    std::optional<SP<Document::Item>> _item;
    double _initialValue;
    Location _initialLocation;
    QMetaObject::Connection _connection;
};

}
} // namespace Editor
} // namespace Lattice
