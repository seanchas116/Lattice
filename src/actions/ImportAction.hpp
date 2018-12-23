#pragma once

#include <QObject>
#include "../app/AppState.hpp"

namespace Lattice {

class ImportAction : public QObject {
    Q_OBJECT
public:
    static void run(QWidget* window, const SP<AppState>& appState);
};

} // namespace Lattice
