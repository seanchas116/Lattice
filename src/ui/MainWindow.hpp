#pragma once
#include "../support/Shorthands.hpp"
#include <QMainWindow>

namespace Lattice {

namespace State {
class AppState;
}

namespace UI {

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

    SP<State::AppState> _appState;
};

} // namespace UI
} // namespace Lattice
