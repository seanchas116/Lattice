#include "AppState.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../document/MeshObject.hpp"
#include "../services/ObjLoader.hpp"
#include "MeshEditState.hpp"
#include <QApplication>
#include <QFileDialog>
#include <QtDebug>
#include <meshlib/Mesh.hpp>
#include <meshlib/builder/CircleBuilder.hpp>
#include <meshlib/builder/ConeBuilder.hpp>
#include <meshlib/builder/CubeBuilder.hpp>
#include <meshlib/builder/CylinderBuilder.hpp>
#include <meshlib/builder/PlaneBuilder.hpp>
#include <meshlib/builder/SphereBuilder.hpp>

using namespace glm;

namespace Lattice {
namespace State {

AppState::AppState() : _document(makeShared<Document::Document>()) {
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

    meshlib::PlaneBuilder builder;
    builder.center = vec3(0);
    builder.size = vec2(2);
    builder.normalAxis = 1;
    object->setMesh(builder.build());

    _document->insertObjectToCurrentPosition(object);
    _document->setCurrentObject(object);
}

void AppState::addCube() {
    _document->history()->beginChange(tr("Add Cube"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Cube").toStdString());

    meshlib::CubeBuilder builder;
    builder.minPos = vec3(-1);
    builder.maxPos = vec3(1);
    object->setMesh(builder.build());

    _document->insertObjectToCurrentPosition(object);
    _document->setCurrentObject(object);
}

void AppState::addCircle() {
    _document->history()->beginChange(tr("Add Circle"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Circle").toStdString());

    meshlib::CircleBuilder builder;
    builder.center = glm::vec3(0);
    builder.radius = 1.0;
    builder.segmentCount = 16;
    builder.normalAxis = 1;
    object->setMesh(builder.build());

    _document->insertObjectToCurrentPosition(object);
    _document->setCurrentObject(object);
}

void AppState::addSphere() {
    _document->history()->beginChange(tr("Add Sphere"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Sphere").toStdString());

    meshlib::SphereBuilder builder;
    builder.center = vec3(0);
    builder.radius = 1;
    builder.segmentCount = 16;
    builder.ringCount = 8;
    builder.axis = 1;
    object->setMesh(builder.build());

    _document->insertObjectToCurrentPosition(object);
    _document->setCurrentObject(object);
}

void AppState::addCone() {
    _document->history()->beginChange(tr("Add Cone"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Cone").toStdString());

    meshlib::ConeBuilder builder;
    builder.center = vec3(0);
    builder.radius = 1.0;
    builder.height = 1.0;
    builder.segmentCount = 16;
    builder.axis = 1;
    object->setMesh(builder.build());

    _document->insertObjectToCurrentPosition(object);
    _document->setCurrentObject(object);
}

void AppState::addCylinder() {
    _document->history()->beginChange(tr("Add Cylinder"));

    auto object = makeShared<Document::MeshObject>();
    object->setName(tr("Cylinder").toStdString());

    meshlib::CylinderBuilder builder;
    builder.center = vec3(0);
    builder.radius = 1;
    builder.height = 1;
    builder.segmentCount = 16;
    builder.axis = 1;
    object->setMesh(builder.build());

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

    auto objects = Services::ObjLoader::load(_document, filePath.toStdString());
    for (auto &object : objects) {
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
        _document->rootObject()->forEachDescendant([&](auto &object) {
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

} // namespace State
} // namespace Lattice
