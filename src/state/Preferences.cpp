#include "Preferences.hpp"

namespace Lattice {
namespace State {

Preferences &Preferences::shared() {
    static Preferences preferences;
    return preferences;
}

} // namespace State
} // namespace Lattice
