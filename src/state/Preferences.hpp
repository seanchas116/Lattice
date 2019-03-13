#pragma once
#include <QObject>

namespace Lattice {
namespace State {

class Preferences : public QObject {
    Q_OBJECT
public:
    double moveThreshold() const { return 4; }
};

} // namespace State
} // namespace Lattice
