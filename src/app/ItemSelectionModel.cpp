#include "ItemSelectionModel.hpp"
#include "ItemModel.hpp"
#include "../document/Document.hpp"
#include <QtDebug>

namespace Lattice {

ItemSelectionModel::ItemSelectionModel(ItemModel *model, QObject *parent) : QItemSelectionModel(model, parent)
{
    connect(this, &QItemSelectionModel::selectionChanged, model, [this, model] {
        std::vector<SP<Item>> items;
        for (auto index : selectedIndexes()) {
            items.push_back(model->itemForIndex(index));
        }
        model->document()->setSelectedItems(items);
    });
}

} // namespace Shapecraft
