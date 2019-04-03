#include "MeshPropertyView.hpp"
#include "../document/MeshObject.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../state/AppState.hpp"
#include "../widget/DoubleSpinBox.hpp"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>

using namespace glm;

namespace Lattice {
namespace UI {

MeshPropertyView::MeshPropertyView(const SP<State::AppState> &appState, QWidget *parent) :
    QWidget(parent),
    _appState(appState)
{
    setObject(appState->document()->editedObject());
    connect(appState->document().get(), &Document::Document::editedObjectChanged, this, &MeshPropertyView::setObject);
    connect(appState->document().get(), &Document::Document::meshSelectionChanged, this, &MeshPropertyView::setViewValues);

    auto layout = new QVBoxLayout();

    {
        auto gridLayout = new QGridLayout();

        std::array<QString, 3> labels {"X", "Y", "Z"};
        for (size_t i = 0; i < 3; ++i) {
            auto label = new QLabel(labels[i]);
            label->setAlignment(Qt::AlignHCenter);
            gridLayout->addWidget(label, 0, int(i + 1));
        }

        auto label = new QLabel(tr("Position"));
        gridLayout->addWidget(label, 1, 0);

        _positionSpinBoxes = {
            new Widget::DoubleSpinBox(),
            new Widget::DoubleSpinBox(),
            new Widget::DoubleSpinBox(),
        };

        for (int i = 0; i < 3; ++i) {
            auto spinBox = _positionSpinBoxes[i];
            spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            spinBox->setMinimum(-std::numeric_limits<double>::infinity());
            spinBox->setMaximum(std::numeric_limits<double>::infinity());
            spinBox->setSpecialValueText(" ");
            gridLayout->addWidget(spinBox, 1, int(i + 1));

            auto handleValueChange = [this, spinBox, i] {
                this->handlePositionValueChange(i, spinBox->value());
            };
            connect(spinBox, &Widget::DoubleSpinBox::editingFinished, this, handleValueChange);
            connect(spinBox, &Widget::DoubleSpinBox::stepped, this, handleValueChange);
        }

        layout->addLayout(gridLayout);
    }

    _smoothEdgeCheckBox = new QCheckBox(tr("Smooth Edge"));
    _smoothEdgeCheckBox->setTristate(true);
    connect(_smoothEdgeCheckBox, &QCheckBox::toggled, this, &MeshPropertyView::handleEdgeSmoothChange);
    layout->addWidget(_smoothEdgeCheckBox);

    layout->addStretch();

    setLayout(layout);
}

void MeshPropertyView::setObject(const Opt<SP<Document::MeshObject>> &object) {
    disconnect(_connection);
    _object = object;
    if (!object) {
        return;
    }

    _connection = connect(object->get(), &Document::MeshObject::meshChangedInLastTick, this, &MeshPropertyView::setViewValues);
    setViewValues();
}

void MeshPropertyView::setViewValues() {
    auto selection = _appState->document()->meshSelection();
    if (selection.vertices.empty()) {
        return;
    }

    {
        glm::bvec3 isPositionSame {true};

        auto position = (*selection.vertices.begin())->position();

        for (auto& vertex : selection.vertices) {
            auto otherPosition = vertex->position();
            for (int i = 0; i < 3; ++i) {
                if (position[i] != otherPosition[i]) {
                    isPositionSame[i] = false;
                }
            }
        }

        auto specialValue = -std::numeric_limits<double>::infinity();

        for (size_t i = 0; i < 3; ++i) {
            _positionSpinBoxes[i]->setValue(isPositionSame[i] ? position[i] : specialValue);
        }
    }

    {
        auto edgeSet = selection.edges();
        std::vector<SP<Mesh::Edge>> edges(edgeSet.begin(), edgeSet.end());
        if (edges.empty()) {
            _smoothEdgeCheckBox->setEnabled(false);
        } else {
            _smoothEdgeCheckBox->setEnabled(true);

            bool isSmoothEdgeSame = true;
            bool isSmooth = edges[0]->isSmooth();

            for (size_t i = 1; i < edges.size(); ++i) {
                if (edges[i]->isSmooth() != isSmooth) {
                    isSmoothEdgeSame = false;
                }
            }

            if (isSmoothEdgeSame) {
                if (isSmooth) {
                    _smoothEdgeCheckBox->setCheckState(Qt::Checked);
                } else {
                    _smoothEdgeCheckBox->setCheckState(Qt::Unchecked);
                }
            } else {
                _smoothEdgeCheckBox->setCheckState(Qt::PartiallyChecked);
            }
        }
    }
}

void MeshPropertyView::handlePositionValueChange(int index, double value) {
    if (!_object) {
        return;
    }
    auto object = *_object;

    auto selection = _appState->document()->meshSelection();
    if (selection.vertices.empty()) {
        return;
    }

    auto specialValue = -std::numeric_limits<double>::infinity();
    if (value == specialValue) {
        return;
    }

    std::unordered_map<SP<Mesh::Vertex>, dvec3> newPositions;
    bool changed = false;

    for (auto& vertex : selection.vertices) {
        auto position = vertex->position();
        if (position[index] == value) {
            continue;
        }
        changed = true;
        position[index] = value;
        newPositions[vertex] = position;
    }
    if (!changed) {
        return;
    }

    _appState->document()->history()->beginChange(tr("Set Vertex Position"));
    object->mesh()->setPosition(newPositions);
}

void MeshPropertyView::handleEdgeSmoothChange(bool smooth) {
    if (!_object) {
        return;
    }
    auto object = *_object;

    auto edges = _appState->document()->meshSelection().edges();
    if (edges.empty()) {
        return;
    }

    _appState->document()->history()->beginChange(tr("Set Edge Smooth"));

    std::unordered_map<SP<Mesh::Edge>, bool> values;
    for (auto& edge : edges) {
        values[edge] = smooth;
    }
    object->mesh()->setSmooth(values);
}

} // namespace UI
} // namespace Lattice
