#pragma once

#include <QWidget>
#include "../support/Pointer.hpp"

namespace Lattice::UI {

class AppState;

class MaterialListPane final : public QWidget {
    Q_OBJECT
public:
    explicit MaterialListPane(const SP<AppState>& appState, QWidget *parent = nullptr);

private:
    SP<AppState> _appState;
};

} // namespace Lattice
