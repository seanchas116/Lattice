#include "ObjectPropertyView.hpp"
#include "../state/AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Object.hpp"
#include "../document/MeshObject.hpp"
#include "../document/History.hpp"
#include "../support/Debug.hpp"
#include "../support/OptionalGuard.hpp"
#include "../widget/SpinBox.hpp"
#include "../widget/DoubleSpinBox.hpp"
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

        std::array<Widget::DoubleSpinBox*, 3> spinBoxes = {
            new Widget::DoubleSpinBox(),
            new Widget::DoubleSpinBox(),
            new Widget::DoubleSpinBox(),
        };

        for (int i = 0; i < 3; ++i) {
            auto spinBox = spinBoxes[i];
            spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            spinBox->setMinimum(-std::numeric_limits<double>::infinity());
            spinBox->setMaximum(std::numeric_limits<double>::infinity());
            spinBox->setSpecialValueText(" ");
            gridLayout->addWidget(spinBox, row, int(i + 1));

            auto handleValueChange = [this, spinBox, member, i] {
                this->handleLocationValueChange(member, i, spinBox->value());
            };
            connect(spinBox, &Widget::DoubleSpinBox::editingFinished, this, handleValueChange);
        }

        return spinBoxes;
    };

    _positionSpinBoxes = buildVec3SpinBoxes(LocationMember::Position, "Position", 1);
    _rotationSpinBoxes = buildVec3SpinBoxes(LocationMember::Rotation, "Rotation", 2);
    _scaleSpinBoxes = buildVec3SpinBoxes(LocationMember::Scale, "Scale", 3);

    layout->addLayout(gridLayout);

    auto subdivLayout = new QFormLayout();

    _subdivEnabledCheckbox = new QCheckBox(tr("Subdivision Surface"));
    _subdivEnabledCheckbox->setTristate(true);
    connect(_subdivEnabledCheckbox, &QCheckBox::toggled, this, &ObjectPropertyView::handleSubdivEnabledChange);
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

    auto location = (*_objects.begin())->location();

    glm::bvec3 isPositionSame {true};
    glm::bvec3 isScaleSame {true};
    glm::bvec3 isRotationSame {true};

    for (auto& object : _objects) {
        auto otherLocation = object->location();
        for (int i = 0; i < 3; ++i) {
            if (location.position[i] != otherLocation.position[i]) {
                isPositionSame[i] = false;
            }
            if (location.scale[i] != otherLocation.scale[i]) {
                isScaleSame[i] = false;
            }
            if (glm::eulerAngles(location.rotation)[i] != glm::eulerAngles(otherLocation.rotation)[i]) {
                isRotationSame[i] = false;
            }
        }
    }

    glm::dvec3 eulerAngles = glm::eulerAngles(location.rotation);

    auto specialValue = -std::numeric_limits<double>::infinity();

    for (size_t i = 0; i < 3; ++i) {
        _positionSpinBoxes[i]->setValue(isPositionSame[i] ? location.position[i] : specialValue);
        _scaleSpinBoxes[i]->setValue(isScaleSame[i] ? location.scale[i] : specialValue);
        _rotationSpinBoxes[i]->setValue(isRotationSame[i] ? glm::degrees(eulerAngles[i]) : specialValue);
    }

    std::vector<SP<Document::MeshObject>> meshObjects;
    for (auto& object : _objects) {
        if (auto meshObject = dynamicPointerCast<Document::MeshObject>(object); meshObject) {
            meshObjects.push_back(*meshObject);
        }
    }

    if (!meshObjects.empty()) {
        _subdivEnabledCheckbox->setVisible(true);

        bool isSubdivEnabledSame = true;
        bool isSubdivEnabled = meshObjects[0]->subdivSettings().isEnabled;
        for (size_t i = 1; i < meshObjects.size(); ++i) {
            if (isSubdivEnabled != meshObjects[i]->subdivSettings().isEnabled) {
                isSubdivEnabledSame = false;
                break;
            }
        }

        if (isSubdivEnabledSame) {
            _subdivEnabledCheckbox->setCheckState(isSubdivEnabled ? Qt::Checked : Qt::Unchecked);
        } else {
            _subdivEnabledCheckbox->setCheckState(Qt::PartiallyChecked);
        }
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
