#pragma once

#include "../support/Shorthands.hpp"
#include <QWidget>
#include <array>

namespace Lattice {

namespace State {
class AppState;
class MeshEditState;
} // namespace State
namespace Document {
class MeshObject;
}
namespace Widget {
class MultiValueDoubleSpinBox;
class MultiValueCheckBox;
} // namespace Widget

namespace UI {

class MeshPropertyView : public QWidget {
    Q_OBJECT
  public:
    explicit MeshPropertyView(const SP<State::AppState> &appState, QWidget *parent = nullptr);

    void setObject(const Opt<SP<Document::MeshObject>> &object);
    void setMeshEditState(const Opt<SP<State::MeshEditState>> &meshEditState);

  private:
    void refreshValues();
    void handlePositionValueChange(int index, double value);
    void handleEdgeSharpChange(bool sharp);
    void handleEdgeCreaseChange(double crease);

    SP<State::AppState> _appState;
    Opt<SP<State::MeshEditState>> _meshEditState;
    QMetaObject::Connection _connection;
    std::array<Widget::MultiValueDoubleSpinBox *, 3> _positionSpinBoxes;
    Widget::MultiValueCheckBox *_edgeSharpCheckBox = nullptr;
    Widget::MultiValueDoubleSpinBox *_edgeCreaseSpinBox = nullptr;
};

} // namespace UI
} // namespace Lattice
