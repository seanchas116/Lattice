#pragma once
#include <QMainWindow>
#include "../support/Shorthands.hpp"

namespace Lattice::UI {

class AppState;

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void setupToolBar();
    void setupMenu();
    void setupPanes();

    SP<AppState> _appState;
};

} // namespace Lattice
