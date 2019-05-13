#include "MeshPropertyView.hpp"
#include "../document/MeshObject.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../state/AppState.hpp"
#include "../state/MeshEditState.hpp"
#include "../widget/DoubleSpinBox.hpp"
#include "../mesh/Mesh.hpp"
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
    setMeshEditState(appState->meshEditState());
    connect(appState.get(), &State::AppState::meshEditStateChanged, this, &MeshPropertyView::setMeshEditState);

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

void MeshPropertyView::setMeshEditState(const Opt<SP<State::MeshEditState> > &meshEditState) {
    disconnect(_connection);
    _meshEditState = meshEditState;
    if (!meshEditState) {
        return;
    }

    _connection = connect(meshEditState->get(), &State::MeshEditState::meshChanged, this, &MeshPropertyView::refreshValues);
    refreshValues();
}

void MeshPropertyView::refreshValues() {
    if (!_meshEditState) {
        return;
    }
    auto meshEditState = *_meshEditState;
    auto& mesh = *meshEditState->mesh();
    auto selection = mesh.selectedVertices() | ranges::to_vector;
    if (selection.empty()) {
        return;
    }

    {
        glm::bvec3 isPositionSame {true};
        auto position = mesh.position(selection[0]);

        for (auto vertex : selection) {
            auto otherPosition = mesh.position(vertex);
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
        auto edges = mesh.edges(selection) | ranges::to_vector;
        if (edges.empty()) {
            _smoothEdgeCheckBox->setEnabled(false);
        } else {
            _smoothEdgeCheckBox->setEnabled(true);

            bool isSmoothEdgeSame = true;
            bool isSmooth = mesh.isSmooth(edges[0]);

            for (size_t i = 1; i < edges.size(); ++i) {
                if (mesh.isSmooth(edges[i]) != isSmooth) {
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
    if (!_meshEditState) {
        return;
    }
    auto meshEditState = *_meshEditState;
    auto& mesh = *meshEditState->mesh();
    auto selection = mesh.selectedVertices() | ranges::to_vector;
    if (selection.empty()) {
        return;
    }

    auto specialValue = -std::numeric_limits<double>::infinity();
    if (value == specialValue) {
        return;
    }

    bool changed = false;

    for (auto vertex : selection) {
        auto position = mesh.position(vertex);
        if (position[index] == value) {
            continue;
        }
        changed = true;
        position[index] = value;
        mesh.setPosition(vertex, position);
    }
    if (!changed) {
        return;
    }

    meshEditState->commitMeshChange(tr("Set Vertex Position"));
}

void MeshPropertyView::handleEdgeSmoothChange(bool smooth) {
    if (!_meshEditState) {
        return;
    }
    auto meshEditState = *_meshEditState;
    auto& mesh = *meshEditState->mesh();
    auto edges = mesh.edges(mesh.selectedVertices()) | ranges::to_vector;
    if (edges.empty()) {
        return;
    }

    for (auto edge : edges) {
        mesh.setSmooth(edge, smooth);
    }

    meshEditState->commitMeshChange(tr("Set Edge Smooth"));
}

} // namespace UI
} // namespace Lattice
