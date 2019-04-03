#include "AppState.hpp"
#include "../services/ObjLoader.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../document/MeshObject.hpp"
#include "../mesh/Mesh.hpp"
#include <QFileDialog>
#include <QtDebug>
#include <QFileInfo>
#include <QApplication>

using namespace glm;

namespace Lattice {
namespace State {

AppState::AppState() :
    _document(makeShared<Document::Document>()),
    _preferences(makeShared<Preferences>())
{
    addCube();
    auto item = _document->rootObject()->childItems()[0];
    _document->setCurrentObject(item);
    _document->history()->clear();
}

void AppState::deleteItems() {
    _document->history()->beginChange(tr("Delete Items"));
    _document->deleteSelectedObjects();
}

void AppState::addPlane() {
    _document->history()->beginChange(tr("Add Plane"));
    auto item = makeShared<Document::MeshObject>();
    item->setName(tr("Plane").toStdString());
    item->mesh()->addPlane(dvec3(0), dvec2(2), 1, item->mesh()->addMaterial());
    _document->insertObjectToCurrentPosition(item);
}

void AppState::addCube() {
    _document->history()->beginChange(tr("Add Cube"));
    auto item = makeShared<Document::MeshObject>();
    item->setName(tr("Cube").toStdString());
    item->mesh()->addCube(glm::vec3(-1), glm::vec3(1), item->mesh()->addMaterial());
    _document->insertObjectToCurrentPosition(item);
}

void AppState::addCircle() {
    _document->history()->beginChange(tr("Add Circle"));
    auto item = makeShared<Document::MeshObject>();
    item->setName(tr("Circle").toStdString());
    item->mesh()->addCircle(glm::vec3(0), 1.0, 16, Mesh::Mesh::CircleFill::Ngon, 1, item->mesh()->addMaterial());
    _document->insertObjectToCurrentPosition(item);
}

void AppState::addSphere() {
    _document->history()->beginChange(tr("Add Sphere"));
    auto item = makeShared<Document::MeshObject>();
    item->setName(tr("Sphere").toStdString());
    item->mesh()->addSphere(glm::vec3(0), 1.0, 16, 8, 1, item->mesh()->addMaterial());
    _document->insertObjectToCurrentPosition(item);
}

void AppState::addCone() {
    _document->history()->beginChange(tr("Add Cone"));
    auto item = makeShared<Document::MeshObject>();
    item->setName(tr("Cone").toStdString());
    item->mesh()->addCone(glm::vec3(0), 1.0, 1.0, 16, 1, item->mesh()->addMaterial());
    _document->insertObjectToCurrentPosition(item);
}

void AppState::addCylinder() {
    _document->history()->beginChange(tr("Add Cylinder"));
    auto item = makeShared<Document::MeshObject>();
    item->setName(tr("Cylinder").toStdString());
    item->mesh()->addCylinder(glm::vec3(0), 1.0, 1.0, 16, 1, item->mesh()->addMaterial());
    _document->insertObjectToCurrentPosition(item);
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
        _document->insertObjectToCurrentPosition(item);
    }
}

void AppState::deleteVertices() {
    auto maybeEditedItem = _document->editedObject();
    if (!maybeEditedItem) {
        return;
    }
    auto editedItem = *maybeEditedItem;

    _document->history()->beginChange(tr("Delete Vertices"));

    auto vertices = _document->meshSelection().vertices;
    for (auto& v : vertices) {
        editedItem->mesh()->removeVertex(v);
    }

    _document->setMeshSelection({});
}

void AppState::deleteEdges() {
    auto maybeEditedItem = _document->editedObject();
    if (!maybeEditedItem) {
        return;
    }
    auto editedItem = *maybeEditedItem;

    _document->history()->beginChange(tr("Delete Edges"));

    auto edges = _document->meshSelection().edges();
    for (auto& e : edges) {
        editedItem->mesh()->removeEdge(e);
    }
}

void AppState::deleteFaces() {
    auto maybeEditedItem = _document->editedObject();
    if (!maybeEditedItem) {
        return;
    }
    auto editedItem = *maybeEditedItem;

    _document->history()->beginChange(tr("Delete Faces"));

    auto faces = _document->meshSelection().faces();
    for (auto& f : faces) {
        editedItem->mesh()->removeFace(f);
    }
}

void AppState::selectAll() {
    auto maybeEditedItem = _document->editedObject();
    if (maybeEditedItem) {
        auto editedItem = *maybeEditedItem;
        Mesh::MeshFragment selection;
        selection.vertices = editedItem->mesh()->vertices();
        _document->setMeshSelection(selection);
    } else {
        std::unordered_set<SP<Document::Object>> allItems;
        _document->rootObject()->forEachDescendant([&] (auto& item) {
            allItems.insert(item);
        });
        _document->setSelectedObjects(allItems);
    }
}

void AppState::deselectAll() {
    auto maybeEditedItem = _document->editedObject();
    if (maybeEditedItem) {
        auto editedItem = *maybeEditedItem;
        Mesh::MeshFragment selection;
        _document->setMeshSelection(selection);
    } else {
        _document->setSelectedObjects({});
    }
}

}
} // namespace Lattice
