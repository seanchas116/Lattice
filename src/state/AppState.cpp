#include "AppState.hpp"
#include "MeshEditState.hpp"
#include "../services/ObjLoader.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../document/MeshObject.hpp"
#include "../oldmesh/Mesh.hpp"
#include "../mesh/Mesh.hpp"
#include "../mesh/algorithm/AddPlane.hpp"
#include "../mesh/algorithm/AddCube.hpp"
#include "../mesh/algorithm/AddCircle.hpp"
#include "../mesh/algorithm/AddSphere.hpp"
#include "../mesh/algorithm/AddCone.hpp"
#include "../mesh/algorithm/AddCylinder.hpp"
#include <QFileDialog>
#include <QtDebug>
#include <QFileInfo>
#include <QApplication>

using namespace glm;

namespace Lattice {
namespace State {

AppState::AppState() :
    _document(makeShared<Document::Document>()),
    _preferences(makeShared<Preferences>()),
    _meshEditState(makeShared<MeshEditState>())
{
    addCube();
    auto object = _document->rootObject()->childObjects()[0];
    _document->setCurrentObject(object);
    _document->history()->clear();
}

void AppState::deleteObjects() {
    _document->history()->beginChange(tr("Delete Objects"));
    _document->deleteSelectedObjects();
}

void AppState::addPlane() {
    _document->history()->beginChange(tr("Add Plane"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Plane").toStdString());

    Mesh::Mesh mesh;
    Mesh::AddPlane(dvec3(0), dvec2(2), 1, 0).redo(mesh);
    object->setMesh(std::move(mesh));

    _document->insertObjectToCurrentPosition(object);
}

void AppState::addCube() {
    _document->history()->beginChange(tr("Add Cube"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Cube").toStdString());

    Mesh::Mesh mesh;
    Mesh::AddCube(glm::vec3(-1), glm::vec3(1), 0).redo(mesh);
    object->setMesh(std::move(mesh));

    _document->insertObjectToCurrentPosition(object);
}

void AppState::addCircle() {
    _document->history()->beginChange(tr("Add Circle"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Circle").toStdString());

    Mesh::Mesh mesh;
    Mesh::AddCircle(glm::vec3(0), 1.0, 16, 1, 0).redo(mesh);
    object->setMesh(std::move(mesh));

    _document->insertObjectToCurrentPosition(object);
}

void AppState::addSphere() {
    _document->history()->beginChange(tr("Add Sphere"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Sphere").toStdString());

    Mesh::Mesh mesh;
    Mesh::AddSphere(glm::vec3(0), 1.0, 16, 8, 1, 0).redo(mesh);
    object->setMesh(std::move(mesh));

    _document->insertObjectToCurrentPosition(object);
}

void AppState::addCone() {
    _document->history()->beginChange(tr("Add Cone"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Cone").toStdString());

    Mesh::Mesh mesh;
    Mesh::AddCone(glm::vec3(0), 1.0, 1.0, 16, 1, 0).redo(mesh);
    object->setMesh(std::move(mesh));

    _document->insertObjectToCurrentPosition(object);
}

void AppState::addCylinder() {
    _document->history()->beginChange(tr("Add Cylinder"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Cylinder").toStdString());

    Mesh::Mesh mesh;
    Mesh::AddCylinder(glm::vec3(0), 1.0, 1.0, 16, 1, 0).redo(mesh);
    object->setMesh(std::move(mesh));

    _document->insertObjectToCurrentPosition(object);
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

    auto objects = Services::ObjLoader::load(filePath);
    _document->history()->beginChange(tr("Import"));
    for (auto& object : objects) {
        _document->insertObjectToCurrentPosition(object);
    }
}

void AppState::deleteVertices() {
    auto maybeEditedObject = _document->editedObject();
    if (!maybeEditedObject) {
        return;
    }
    auto editedObject = *maybeEditedObject;

    _document->history()->beginChange(tr("Delete Vertices"));

    auto vertices = _document->meshSelection().vertices;
    for (auto& v : vertices) {
        editedObject->oldMesh()->removeVertex(v);
    }

    _document->setMeshSelection({});
}

void AppState::deleteEdges() {
    auto maybeEditedObject = _document->editedObject();
    if (!maybeEditedObject) {
        return;
    }
    auto editedObject = *maybeEditedObject;

    _document->history()->beginChange(tr("Delete Edges"));

    auto edges = _document->meshSelection().edges();
    for (auto& e : edges) {
        editedObject->oldMesh()->removeEdge(e);
    }
}

void AppState::deleteFaces() {
    auto maybeEditedObject = _document->editedObject();
    if (!maybeEditedObject) {
        return;
    }
    auto editedObject = *maybeEditedObject;

    _document->history()->beginChange(tr("Delete Faces"));

    auto faces = _document->meshSelection().faces();
    for (auto& f : faces) {
        editedObject->oldMesh()->removeFace(f);
    }
}

void AppState::selectAll() {
    auto maybeEditedObject = _document->editedObject();
    if (maybeEditedObject) {
        auto editedObject = *maybeEditedObject;
        OldMesh::MeshFragment selection;
        selection.vertices = editedObject->oldMesh()->vertices();
        _document->setMeshSelection(selection);
    } else {
        std::unordered_set<SP<Document::Object>> allObjects;
        _document->rootObject()->forEachDescendant([&] (auto& object) {
            allObjects.insert(object);
        });
        _document->setSelectedObjects(allObjects);
    }
}

void AppState::deselectAll() {
    auto maybeEditedObject = _document->editedObject();
    if (maybeEditedObject) {
        auto editedObject = *maybeEditedObject;
        OldMesh::MeshFragment selection;
        _document->setMeshSelection(selection);
    } else {
        _document->setSelectedObjects({});
    }
}

}
} // namespace Lattice
