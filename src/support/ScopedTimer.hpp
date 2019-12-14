#pragma once
#include <QElapsedTimer>
#include <QString>

namespace Lattice {

class ScopedTimer final {
    Q_DISABLE_COPY(ScopedTimer)
  public:
    ScopedTimer(const std::string &title);
    ~ScopedTimer();

  private:
    QElapsedTimer _timer;
    std::string _title;
};

} // namespace Lattice
