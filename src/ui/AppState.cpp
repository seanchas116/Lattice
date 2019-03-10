#include "AppState.hpp"
#include "../services/ObjLoader.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../document/MeshItem.hpp"
#include "../mesh/Mesh.hpp"
#include <QFileDialog>
#include <QtDebug>
#include <QFileInfo>
#include <QApplication>

using namespace glm;

namespace Lattice::UI {

AppState::AppState() : _document(makeShared<Document::Document>()) {
    addCube();
    auto item = _document->rootItem()->childItems()[0];
    _document->setCurrentItem(item);
    _document->history()->clear();
}

void AppState::deleteItems() {
    _document->history()->beginChange(tr("Delete Items"));
    _document->deleteSelectedItems();
}

void AppState::addPlane() {
    _document->history()->beginChange(tr("Add Plane"));
    auto item = makeShared<Document::MeshItem>();
    item->setName(tr("Plane").toStdString());
    item->mesh()->addPlane(dvec3(0), dvec2(2), 1, item->mesh()->addMaterial());
    _document->insertItemToCurrentPosition(item);
}

void AppState::addCube() {
    _document->history()->beginChange(tr("Add Cube"));
    auto item = makeShared<Document::MeshItem>();
    item->setName(tr("Cube").toStdString());
    item->mesh()->addCube(glm::vec3(-1), glm::vec3(1), item->mesh()->addMaterial());
    _document->insertItemToCurrentPosition(item);
}

void AppState::addCircle() {
    _document->history()->beginChange(tr("Add Circle"));
    auto item = makeShared<Document::MeshItem>();
    item->setName(tr("Circle").toStdString());
    item->mesh()->addCircle(glm::vec3(0), 1.0, 16, Document::Mesh::CircleFill::Ngon, 1, item->mesh()->addMaterial());
    _document->insertItemToCurrentPosition(item);
}

void AppState::addSphere() {
    _document->history()->beginChange(tr("Add Sphere"));
    auto item = makeShared<Document::MeshItem>();
    item->setName(tr("Sphere").toStdString());
    item->mesh()->addSphere(glm::vec3(0), 1.0, 16, 8, 1, item->mesh()->addMaterial());
    _document->insertItemToCurrentPosition(item);
}

void AppState::addCone() {
    _document->history()->beginChange(tr("Add Cone"));
    auto item = makeShared<Document::MeshItem>();
    item->setName(tr("Cone").toStdString());
    item->mesh()->addCone(glm::vec3(0), 1.0, 1.0, 16, 1, item->mesh()->addMaterial());
    _document->insertItemToCurrentPosition(item);
}

void AppState::addCylinder() {
    _document->history()->beginChange(tr("Add Cylinder"));
    auto item = makeShared<Document::MeshItem>();
    item->setName(tr("Cylinder").toStdString());
    item->mesh()->addCylinder(glm::vec3(0), 1.0, 1.0, 16, 1, item->mesh()->addMaterial());
    _document->insertItemToCurrentPosition(item);
}

void AppState::addText() {

}

void AppState::addImagePlane() {

}

void AppState::import() {
    auto filePath = QFileDialog::getOpenFileName(qApp->activeWindow(), tr("Import"), {}, tr("Files (*.obj)"));
    if (filePath.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(filePath);

    auto items = Services::ObjLoader::load(filePath);
    _document->history()->beginChange(tr("Import"));
    for (auto& item: items) {
        _document->insertItemToCurrentPosition(item);
    }
}

} // namespace Lattice
