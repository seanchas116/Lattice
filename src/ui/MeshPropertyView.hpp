#pragma once

#include "../support/Shorthands.hpp"
#include <QWidget>
#include <array>

class QCheckBox;

namespace Lattice {

namespace State {
class AppState;
class MeshEditState;
}
namespace Document {
class MeshObject;
}
namespace Widget {
class DoubleSpinBox;
}

namespace UI {

class MeshPropertyView : public QWidget {
    Q_OBJECT
public:
    explicit MeshPropertyView(const SP<State::AppState> &appState, QWidget *parent = nullptr);

    void setObject(const Opt<SP<Document::MeshObject>>& object);
    void setMeshEditState(const Opt<SP<State::MeshEditState>>& meshEditState);

private:
    void setViewValues();
    void handlePositionValueChange(int index, double value);
    void handleEdgeSmoothChange(bool smooth);

    SP<State::AppState> _appState;
    Opt<SP<State::MeshEditState>> _meshEditState;
    QMetaObject::Connection _connection;
    std::array<Widget::DoubleSpinBox*, 3> _positionSpinBoxes;
    QCheckBox* _smoothEdgeCheckBox = nullptr;
};

} // namespace UI
} // namespace Lattice
