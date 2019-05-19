#pragma once

#include "../support/Shorthands.hpp"
#include <QWidget>
#include <array>
#include <unordered_set>

class QCheckBox;

namespace Lattice {

namespace State {
class AppState;
}

namespace Widget {
class SpinBox;
class MultiValueCheckBox;
class MultiValueDoubleSpinBox;
}

namespace Document {
class Object;
}

namespace UI {

class ObjectPropertyView final : public QWidget {
    Q_OBJECT
public:
    explicit ObjectPropertyView(const SP<State::AppState> &appState, QWidget *parent = nullptr);

    void setObjects(const std::unordered_set<SP<Document::Object>> &objects);
    auto& objects() const { return _objects; }

private:
    enum class LocationMember {
        Position, Scale, Rotation,
    };

    void refreshValues();

    void handleLocationValueChange(LocationMember member, int index, double value);
    void handleSubdivEnabledChange(bool enabled);

    SP<State::AppState> _appState;

    std::unordered_set<SP<Document::Object>> _objects;
    std::vector<QMetaObject::Connection> _connections;

    std::array<Widget::MultiValueDoubleSpinBox*, 3> _positionSpinBoxes;
    std::array<Widget::MultiValueDoubleSpinBox*, 3> _scaleSpinBoxes;
    std::array<Widget::MultiValueDoubleSpinBox*, 3> _rotationSpinBoxes;

    Widget::MultiValueCheckBox* _subdivEnabledCheckbox = nullptr;
    Widget::SpinBox* _subdivSegmentCountSpinbox = nullptr;
};

}
} // namespace Lattice
