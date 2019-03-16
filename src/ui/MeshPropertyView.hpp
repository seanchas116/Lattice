#pragma once

#include "../support/Shorthands.hpp"
#include <QWidget>

namespace Lattice {

namespace State {
class AppState;
}
namespace Document {
class MeshItem;
}

namespace UI {

class MeshPropertyView : public QWidget {
    Q_OBJECT
public:
    explicit MeshPropertyView(const SP<State::AppState> &appState, QWidget *parent = nullptr);

private:
    void setItem(const Opt<SP<Document::MeshItem>>& item);
    void updateViews();

    SP<State::AppState> _appState;
    Opt<SP<Document::MeshItem>> _item;
    QMetaObject::Connection _connection;
};

} // namespace UI
} // namespace Lattice
