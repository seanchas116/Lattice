#pragma once
#include <QObject>

namespace Lattice {
namespace State {

class Preferences : public QObject {
    Q_OBJECT
public:
    Preferences();
};

} // namespace State
} // namespace Lattice
