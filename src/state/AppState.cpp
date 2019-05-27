#include "AppState.hpp"
#include "MeshEditState.hpp"
#include "../services/ObjLoader.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../document/MeshObject.hpp"
#include "../mesh/Mesh.hpp"
#include "../mesh/algorithm/BuildPlane.hpp"
#include "../mesh/algorithm/BuildCube.hpp"
#include "../mesh/algorithm/BuildCircle.hpp"
#include "../mesh/algorithm/BuildSphere.hpp"
#include "../mesh/algorithm/BuildCone.hpp"
#include "../mesh/algorithm/BuildCylinder.hpp"
#include <QFileDialog>
#include <QtDebug>
#include <QFileInfo>
#include <QApplication>

using namespace glm;

namespace Lattice {
namespace State {

AppState::AppState() :
    _document(makeShared<Document::Document>())
{
    addCube();
    auto object = _document->rootObject()->childObjects()[0];
    _document->setCurrentObject(object);
    _document->history()->clear();

    connect(_document.get(), &Document::Document::currentObjectChanged, this, &AppState::endEditing);
}

void AppState::deleteObjects() {
    _document->history()->beginChange(tr("Delete Objects"));
    _document->deleteSelectedObjects();
}

void AppState::addPlane() {
    _document->history()->beginChange(tr("Add Plane"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Plane").toStdString());

    auto mesh = Mesh::BuildPlane(dvec3(0), dvec2(2), 1, Mesh::MaterialHandle()).perform();
    object->setMesh(std::move(mesh));

    _document->insertObjectToCurrentPosition(object);
    _document->setCurrentObject(object);
}

void AppState::addCube() {
    _document->history()->beginChange(tr("Add Cube"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Cube").toStdString());

    auto mesh = Mesh::BuildCube(glm::vec3(-1), glm::vec3(1), Mesh::MaterialHandle()).perform();
    object->setMesh(std::move(mesh));

    _document->insertObjectToCurrentPosition(object);
    _document->setCurrentObject(object);
}

void AppState::addCircle() {
    _document->history()->beginChange(tr("Add Circle"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Circle").toStdString());

    auto mesh = Mesh::BuildCircle(glm::vec3(0), 1.0, 16, 1, Mesh::MaterialHandle()).perform();
    object->setMesh(std::move(mesh));

    _document->insertObjectToCurrentPosition(object);
    _document->setCurrentObject(object);
}

void AppState::addSphere() {
    _document->history()->beginChange(tr("Add Sphere"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Sphere").toStdString());

    auto mesh = Mesh::BuildSphere(glm::vec3(0), 1.0, 16, 8, 1, Mesh::MaterialHandle()).perform();
    object->setMesh(std::move(mesh));

    _document->insertObjectToCurrentPosition(object);
    _document->setCurrentObject(object);
}

void AppState::addCone() {
    _document->history()->beginChange(tr("Add Cone"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Cone").toStdString());

    auto mesh = Mesh::BuildCone(glm::vec3(0), 1.0, 1.0, 16, 1, Mesh::MaterialHandle()).perform();
    object->setMesh(std::move(mesh));

    _document->insertObjectToCurrentPosition(object);
    _document->setCurrentObject(object);
}

void AppState::addCylinder() {
    _document->history()->beginChange(tr("Add Cylinder"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Cylinder").toStdString());

    auto mesh = Mesh::BuildCylinder(glm::vec3(0), 1.0, 1.0, 16, 1, Mesh::MaterialHandle()).perform();
    object->setMesh(std::move(mesh));

    _document->insertObjectToCurrentPosition(object);
    _document->setCurrentObject(object);
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

    _document->history()->beginChange(tr("Import"));

    auto objects = Services::ObjLoader::load(_document, filePath);
    for (auto& object : objects) {
        _document->insertObjectToCurrentPosition(object);
    }
}

void AppState::deleteVertices() {
    if (_meshEditState) {
        (*_meshEditState)->deleteVertices();
    }
}

void AppState::deleteEdges() {
    if (_meshEditState) {
        (*_meshEditState)->deleteEdges();
    }
}

void AppState::deleteFaces() {
    if (_meshEditState) {
        (*_meshEditState)->deleteFaces();
    }
}

void AppState::selectAll() {
    if (_meshEditState) {
        (*_meshEditState)->selectAll();
    } else {
        std::unordered_set<SP<Document::Object>> allObjects;
        _document->rootObject()->forEachDescendant([&] (auto& object) {
            allObjects.insert(object);
        });
        _document->setSelectedObjects(std::move(allObjects));
    }
}

void AppState::deselectAll() {
    if (_meshEditState) {
        (*_meshEditState)->deselectAll();
    } else {
        _document->setSelectedObjects({});
    }
}

void AppState::startEditing() {
    auto currentMeshObject = dynamicPointerCast<Document::MeshObject>(_document->currentObject());
    if (currentMeshObject) {
        startEditing(*currentMeshObject);
    }
}

void AppState::endEditing() {
    setMeshEditState({});
}

void AppState::startEditing(const SP<Document::MeshObject> &object) {
    setMeshEditState(makeShared<MeshEditState>(object));
}

}
} // namespace Lattice
