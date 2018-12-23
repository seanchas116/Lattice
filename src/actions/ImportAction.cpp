#include "ImportAction.hpp"
#include "../services/ObjLoader.hpp"
#include "../document/Document.hpp"
#include "../document/MeshItem.hpp"
#include <QFileDialog>
#include <QtDebug>
#include <QFileInfo>

namespace Lattice {

void ImportAction::run(QWidget* window, const SP<AppState> &appState) {
    auto filePath = QFileDialog::getOpenFileName(window, tr("Import"), {}, tr("Files (*.obj)"));
    qDebug() << filePath;

    QFileInfo fileInfo(filePath);

    auto items = ObjLoader::load(filePath.toStdString());
    for (auto& item: items) {
        appState->document()->insertItemToCurrentPosition(item);
    }
}

} // namespace Lattice
