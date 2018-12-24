#include "ItemPropertyPane.hpp"
#include "AppState.hpp"
#include "../document/Document.hpp"
#include "../document/Item.hpp"
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QtDebug>

namespace Lattice {

ItemPropertyPane::ItemPropertyPane(const SP<AppState> &appState, QWidget *parent) :
    QWidget(parent), _appState(appState)
{
    auto layout = new QVBoxLayout();

    auto positionLayout = new QGridLayout();

    _posXSpinBox = new QDoubleSpinBox();
    _posYSpinBox = new QDoubleSpinBox();
    _posZSpinBox = new QDoubleSpinBox();
    for (auto spinBox : {_posXSpinBox, _posYSpinBox, _posZSpinBox}) {
        spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        spinBox->setMinimum(-100);
        spinBox->setMaximum(100);
    }

    connect(appState->document().get(), &Document::selectedItemsChanged, this, &ItemPropertyPane::onSelectedItemChanged);
    onSelectedItemChanged();

    /*
    connect(editorState.get(), &EditorState::selectionChanged, this, updatePosition);
    // TODO: support current item changes
    connect(editorState->focusedItem().get(), &Item::changed, this, updatePosition);

    auto setPosition = [=] {
        auto x = xSpinBox->value();
        auto y = ySpinBox->value();
        auto z = zSpinBox->value();
        _editorState->selection().setMedianPosition(glm::vec3(x, y, z));
        _editorState->focusedItem()->notifyChanged();
    };
    connect(xSpinBox, &QDoubleSpinBox::editingFinished, this, setPosition);
    connect(ySpinBox, &QDoubleSpinBox::editingFinished, this, setPosition);
    connect(zSpinBox, &QDoubleSpinBox::editingFinished, this, setPosition);
    */

    positionLayout->addWidget(_posXSpinBox, 0, 0);
    positionLayout->addWidget(_posYSpinBox, 0, 1);
    positionLayout->addWidget(_posZSpinBox, 0, 2);

    auto xLabel = new QLabel("X");
    xLabel->setAlignment(Qt::AlignHCenter);
    auto yLabel = new QLabel("Y");
    yLabel->setAlignment(Qt::AlignHCenter);
    auto zLabel = new QLabel("Z");
    zLabel->setAlignment(Qt::AlignHCenter);

    positionLayout->addWidget(xLabel, 1, 0);
    positionLayout->addWidget(yLabel, 1, 1);
    positionLayout->addWidget(zLabel, 1, 2);

    layout->addLayout(positionLayout);

    layout->addStretch();

    setLayout(layout);
}

void ItemPropertyPane::onSelectedItemChanged() {
    for (auto& c : _itemConnections) {
        disconnect(c);
    }
    _itemConnections.clear();

    auto selectedItems = _appState->document()->selectedItems();
    for (auto& item : selectedItems) {
        auto c = connect(item.get(), &Item::locationChanged, this, &ItemPropertyPane::onLocationChanged);
        _itemConnections.push_back(c);
    }
    onLocationChanged();
}

void ItemPropertyPane::onLocationChanged() {
    // TODO: support multiple items
    // TODO: spinboxes must be disbled when no item is selected

    auto selectedItems = _appState->document()->selectedItems();
    auto pos = selectedItems.empty() ? glm::vec3(-1) : selectedItems[0]->location().position;

    _posXSpinBox->setValue(double(pos.x));
    _posYSpinBox->setValue(double(pos.y));
    _posZSpinBox->setValue(double(pos.z));
}

} // namespace Lattice
