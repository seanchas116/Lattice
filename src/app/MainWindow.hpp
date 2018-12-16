#pragma once
#include <QMainWindow>
#include "../support/Pointer.hpp"

namespace Lattice {

class AppState;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void setupActions();

    SP<AppState> _appState;
};

} // namespace Lattice
