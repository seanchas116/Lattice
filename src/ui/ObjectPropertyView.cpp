#include "ObjectPropertyView.hpp"
#include "../state/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Object.hpp"
#include "../document/MeshObject.hpp"
#include "../document/History.hpp"
#include "../support/Debug.hpp"
#include "../support/OptionalGuard.hpp"
#include "../widget/SpinBox.hpp"
#include "../widget/MultiValueCheckBox.hpp"
#include "../widget/MultiValueDoubleSpinBox.hpp"
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QCheckBox>
#include <QtDebug>

namespace Lattice {
namespace UI {

ObjectPropertyView::ObjectPropertyView(const SP<State::AppState> &appState, QWidget *parent) :
    QWidget(parent),
    _appState(appState)
{
    auto layout = new QVBoxLayout();

    auto gridLayout = new QGridLayout();

    std::array<QString, 3> labels {"X", "Y", "Z"};
    for (size_t i = 0; i < 3; ++i) {
        auto label = new QLabel(labels[i]);
        label->setAlignment(Qt::AlignHCenter);
        gridLayout->addWidget(label, 0, int(i + 1));
    }

    auto buildVec3SpinBoxes = [&] (LocationMember member, const QString& title, int row) {
        auto label = new QLabel(title);
        gridLayout->addWidget(label, row, 0);

        std::array<Widget::MultiValueDoubleSpinBox*, 3> spinBoxes = {
            new Widget::MultiValueDoubleSpinBox(),
            new Widget::MultiValueDoubleSpinBox(),
            new Widget::MultiValueDoubleSpinBox(),
        };

        for (int i = 0; i < 3; ++i) {
            auto spinBox = spinBoxes[i];
            spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            gridLayout->addWidget(spinBox, row, int(i + 1));

            auto handleValueChange = [this, member, i] (double value) {
                this->handleLocationValueChange(member, i, value);
            };
            connect(spinBox, &Widget::MultiValueDoubleSpinBox::editingFinished, this, handleValueChange);
        }

        return spinBoxes;
    };

    _positionSpinBoxes = buildVec3SpinBoxes(LocationMember::Position, "Position", 1);
    _rotationSpinBoxes = buildVec3SpinBoxes(LocationMember::Rotation, "Rotation", 2);
    _scaleSpinBoxes = buildVec3SpinBoxes(LocationMember::Scale, "Scale", 3);

    layout->addLayout(gridLayout);

    auto subdivLayout = new QFormLayout();

    _subdivEnabledCheckbox = new Widget::MultiValueCheckBox(tr("Subdivision Surface"));
    connect(_subdivEnabledCheckbox, &Widget::MultiValueCheckBox::clicked, this, &ObjectPropertyView::handleSubdivEnabledChange);
    subdivLayout->addRow(_subdivEnabledCheckbox);

    _subdivSegmentCountSpinbox = new Widget::SpinBox();
    _subdivSegmentCountSpinbox->setRange(1, 8);
    subdivLayout->addRow(tr("Segment Count"), _subdivSegmentCountSpinbox);

    layout->addLayout(subdivLayout);

    layout->addStretch();

    setLayout(layout);
}

void ObjectPropertyView::setObjects(const std::unordered_set<SP<Document::Object> > &objects) {
    if (_objects == objects) {
        return;
    }
    _objects = objects;

    Opt<SP<Document::Object>> firstObject;
    if (!objects.empty()) {
        firstObject = *objects.begin();
    }

    for (auto& c : _connections) {
        disconnect(c);
    }
    _connections.clear();

    for (auto& object : objects) {
        _connections.push_back(connect(object.get(), &Document::Object::locationChanged, this, &ObjectPropertyView::refreshValues));
        if (auto maybeMeshObject = dynamicPointerCast<Document::MeshObject>(object); maybeMeshObject) {
            auto meshObject = *maybeMeshObject;
            _connections.push_back(connect(meshObject.get(), &Document::MeshObject::subdivSettingsChanged, this, &ObjectPropertyView::refreshValues));
        }
    }
    refreshValues();
}

void ObjectPropertyView::refreshValues() {
    if (_objects.empty()) {
        return;
    }

    std::array<std::vector<double>, 3> positionValueArrays;
    std::array<std::vector<double>, 3> scaleValueArrays;
    std::array<std::vector<double>, 3> rotationValueArrays;

    for (auto& object : _objects) {
        auto& location = object->location();
        auto eulerAngles = glm::eulerAngles(location.rotation);
        for (int i = 0; i < 3; ++i) {
            positionValueArrays[i].push_back(location.position[i]);
            scaleValueArrays[i].push_back(location.scale[i]);
            rotationValueArrays[i].push_back(glm::degrees(eulerAngles[i]));
        }
    }

    for (size_t i = 0; i < 3; ++i) {
        _positionSpinBoxes[i]->setValues(positionValueArrays[i]);
        _scaleSpinBoxes[i]->setValues(scaleValueArrays[i]);
        _rotationSpinBoxes[i]->setValues(rotationValueArrays[i]);
    }

    std::vector<SP<Document::MeshObject>> meshObjects;
    for (auto& object : _objects) {
        if (auto meshObject = dynamicPointerCast<Document::MeshObject>(object); meshObject) {
            meshObjects.push_back(*meshObject);
        }
    }

    if (!meshObjects.empty()) {
        _subdivEnabledCheckbox->setVisible(true);

        std::vector<bool> subdivEnabledValues;
        for (auto& meshObject : meshObjects) {
            subdivEnabledValues.push_back(meshObject->subdivSettings().isEnabled);
        }
        _subdivEnabledCheckbox->setValues(subdivEnabledValues);
    } else {
        _subdivEnabledCheckbox->setVisible(false);
    }
}

void ObjectPropertyView::handleLocationValueChange(LocationMember member, int index, double value) {
    if (_objects.empty()) {
        return;
    }
    auto specialValue = -std::numeric_limits<double>::infinity();
    if (value == specialValue) {
        return;
    }

    _appState->document()->history()->beginChange(tr("Set Object Location"));
    for (auto& object : _objects) {
        auto location = object->location();
        auto eulerAngles = glm::eulerAngles(location.rotation);

        switch (member) {
        case LocationMember::Position:
            location.position[index] = value;
            break;
        case LocationMember::Scale:
            location.scale[index] = value;
            break;
        case LocationMember::Rotation:
            eulerAngles[index] = glm::radians(glm::mod(value, 360.0));
            break;
        }
        location.rotation = glm::normalize(glm::dquat(eulerAngles));
        object->setLocation(location);
    }
}

void ObjectPropertyView::handleSubdivEnabledChange(bool enabled) {
    std::vector<SP<Document::MeshObject>> meshObjects;
    for (auto& object : _objects) {
        if (auto meshObject = dynamicPointerCast<Document::MeshObject>(object); meshObject) {
            meshObjects.push_back(*meshObject);
        }
    }
    if (meshObjects.empty()) {
        return;
    }

    _appState->document()->history()->beginChange(tr("Set Subdiv Enabled"));
    for (auto& object : meshObjects) {
        auto settings = object->subdivSettings();
        settings.isEnabled = enabled;
        object->setSubdivSettings(settings);
    }
}

}
} // namespace Lattice
