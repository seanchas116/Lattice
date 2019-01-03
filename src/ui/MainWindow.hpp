#pragma once
#include <QMainWindow>
#include "../support/Pointer.hpp"

namespace Lattice::UI {

class AppState;

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupToolBar();
    void setupMenu();
    void setupPanes();

    SP<AppState> _appState;
};

} // namespace Lattice
