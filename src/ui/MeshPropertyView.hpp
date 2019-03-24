#pragma once

#include "../support/Shorthands.hpp"
#include <QWidget>
#include <array>

class QCheckBox;

namespace Lattice {

namespace State {
class AppState;
}
namespace Document {
class MeshItem;
}
namespace Widget {
class DoubleSpinBox;
}

namespace UI {

class MeshPropertyView : public QWidget {
    Q_OBJECT
public:
    explicit MeshPropertyView(const SP<State::AppState> &appState, QWidget *parent = nullptr);

    void setItem(const Opt<SP<Document::MeshItem>>& item);

private:
    void setViewValues();
    void handlePositionValueChange(int index, double value);

    SP<State::AppState> _appState;
    Opt<SP<Document::MeshItem>> _item;
    QMetaObject::Connection _connection;
    std::array<Widget::DoubleSpinBox*, 3> _positionSpinBoxes;
    QCheckBox* _smoothEdgeCheckBox = nullptr;
};

} // namespace UI
} // namespace Lattice
