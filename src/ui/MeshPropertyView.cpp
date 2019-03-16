#include "MeshPropertyView.hpp"
#include "../document/MeshItem.hpp"
#include "../document/Document.hpp"
#include "../state/AppState.hpp"
#include "../widget/DoubleSpinBox.hpp"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

namespace Lattice {
namespace UI {

MeshPropertyView::MeshPropertyView(const SP<State::AppState> &appState, QWidget *parent) :
    QWidget(parent),
    _appState(appState)
{
    setItem(appState->document()->editedItem());
    connect(appState->document().get(), &Document::Document::editedItemChanged, this, &MeshPropertyView::setItem);
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
        }

        layout->addLayout(gridLayout);
    }
    layout->addStretch();

    setLayout(layout);
}

void MeshPropertyView::setItem(const Opt<SP<Document::MeshItem>> &maybeItem) {
    disconnect(_connection);
    _item = maybeItem;
    if (!maybeItem) {
        return;
    }
    auto item = *maybeItem;

    _connection = connect(item->mesh().get(), &Mesh::Mesh::changed, this, [this] {
        setViewValues();
    });
    setViewValues();
}

void MeshPropertyView::setViewValues() {
    auto selection = _appState->document()->meshSelection();
    if (selection.vertices.empty()) {
        return;
    }

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

} // namespace UI
} // namespace Lattice
