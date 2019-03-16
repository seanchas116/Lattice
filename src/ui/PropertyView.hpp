#pragma once
#include "../support/Shorthands.hpp"
#include <QWidget>
#include <unordered_set>

namespace Lattice {

namespace State {
class AppState;
}
namespace Document {
class Item;
}

namespace UI {

class ItemPropertyView;

class PropertyView : public QWidget {
    Q_OBJECT
public:
    explicit PropertyView(const SP<State::AppState> &appState, QWidget *parent = nullptr);

private:
    void handleSelectedItemsChanged(const std::unordered_set<SP<Document::Item>>& items);

    SP<State::AppState> _appState;
    ItemPropertyView* _itemPropertyView {nullptr};
};

} // namespace UI
} // namespace Lattice
