#pragma once
#include <QObject>

namespace Lattice {
namespace State {

class Preferences : public QObject {
    Q_OBJECT
public:
    double moveThreshold() const { return 4; }

    static Preferences& shared();
};

} // namespace State
} // namespace Lattice
