#include "MeshPropertyView.hpp"
#include "../document/MeshObject.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../state/AppState.hpp"
#include "../state/MeshEditState.hpp"
#include "../widget/MultiValueDoubleSpinBox.hpp"
#include "../widget/MultiValueCheckBox.hpp"
#include "../mesh/Mesh.hpp"
#include <QFormLayout>
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

    auto layout = new QFormLayout();

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
            new Widget::MultiValueDoubleSpinBox(),
            new Widget::MultiValueDoubleSpinBox(),
            new Widget::MultiValueDoubleSpinBox(),
        };

        for (int i = 0; i < 3; ++i) {
            auto spinBox = _positionSpinBoxes[i];
            spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            gridLayout->addWidget(spinBox, 1, int(i + 1));

            auto handleValueChange = [this, i] (double value) {
                this->handlePositionValueChange(i, value);
            };
            connect(spinBox, &Widget::MultiValueDoubleSpinBox::editingFinished, this, handleValueChange);
        }

        layout->addRow(gridLayout);
    }

    _edgeSmoothCheckBox = new Widget::MultiValueCheckBox(tr("Edge Smooth"));
    connect(_edgeSmoothCheckBox, &Widget::MultiValueCheckBox::clicked, this, &MeshPropertyView::handleEdgeSmoothChange);
    layout->addRow(_edgeSmoothCheckBox);

    _edgeCreaseSpinBox = new Widget::MultiValueDoubleSpinBox();
    connect(_edgeCreaseSpinBox, &Widget::MultiValueDoubleSpinBox::editingFinished, this, &MeshPropertyView::handleEdgeCreaseChange);
    layout->addRow(tr("Edge Crease"), _edgeCreaseSpinBox);

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
        std::array<std::vector<double>, 3> positionValueArrays;
        for (size_t i = 0; i < 3; ++i) {
            positionValueArrays[i].reserve(selection.size());
        }

        for (auto vertex : selection) {
            auto position = mesh.position(vertex);
            for (int i = 0; i < 3; ++i) {
                positionValueArrays[i].push_back(position[i]);
            }
        }

        for (size_t i = 0; i < 3; ++i) {
            _positionSpinBoxes[i]->setValues(positionValueArrays[i]);
        }
    }

    {
        auto edges = mesh.edges(selection) | ranges::to_vector;
        if (edges.empty()) {
            _edgeSmoothCheckBox->setEnabled(false);
            _edgeCreaseSpinBox->setEnabled(false);
        } else {
            _edgeSmoothCheckBox->setEnabled(true);
            _edgeCreaseSpinBox->setEnabled(true);

            std::vector<bool> isSmoothValues;
            std::vector<double> creaseValues;
            for (auto edge : edges) {
                isSmoothValues.push_back(mesh.isSmooth(edge));
                creaseValues.push_back(mesh.crease(edge));
            }
            _edgeSmoothCheckBox->setValues(isSmoothValues);
            _edgeCreaseSpinBox->setValues(creaseValues);
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

    meshEditState->commitMeshChanged(tr("Set Vertex Position"));
}

void MeshPropertyView::handleEdgeSmoothChange(bool smooth) {
    if (!_meshEditState) {
        return;
    }
    auto meshEditState = *_meshEditState;
    auto& mesh = *meshEditState->mesh();
    auto edges = mesh.selectedEdges() | ranges::to_vector;
    if (edges.empty()) {
        return;
    }
    for (auto edge : edges) {
        mesh.setSmooth(edge, smooth);
    }

    meshEditState->commitMeshChanged(tr("Set Edge Smooth"));
}

void MeshPropertyView::handleEdgeCreaseChange(double crease) {
    if (!_meshEditState) {
        return;
    }
    auto meshEditState = *_meshEditState;
    auto& mesh = *meshEditState->mesh();
    auto edges = mesh.selectedEdges() | ranges::to_vector;
    if (edges.empty()) {
        return;
    }

    for (auto edge : edges) {
        mesh.setCrease(edge, float(crease));
    }

    meshEditState->commitMeshChanged(tr("Set Edge Crease"));
}

} // namespace UI
} // namespace Lattice
