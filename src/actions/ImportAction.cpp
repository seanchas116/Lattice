#include "ImportAction.hpp"
#include "../services/ObjLoader.hpp"
#include <QFileDialog>
#include <QtDebug>

namespace Lattice {

void ImportAction::run(QWidget* window, const SP<AppState> &appState) {
    auto filePath = QFileDialog::getOpenFileName(window, tr("Import"), {}, tr("Files (*.obj)"));
    qDebug() << filePath;
}

} // namespace Lattice
