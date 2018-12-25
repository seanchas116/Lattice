#include "ItemSelectionModel.hpp"
#include "ItemModel.hpp"
#include "../document/Document.hpp"
#include <QtDebug>

namespace Lattice {

ItemSelectionModel::ItemSelectionModel(ItemModel *model, QObject *parent) : QItemSelectionModel(model, parent)
{
    auto onSelectionChange = [this, model] {
        QItemSelection selection;
        for (auto item : model->document()->selectedItems()) {
            auto index = model->indexForItem(item);
            selection.select(index, index);
        }
        select(selection, QItemSelectionModel::ClearAndSelect);
    };
    auto onCurrentChange = [this, model] {
        auto currentItem = model->document()->currentItem();
        if (currentItem) {
            auto current = model->indexForItem(currentItem);
            select(current, QItemSelectionModel::Current);
        } else {
            clearCurrentIndex();
        }
    };
    connect(model->document().get(), &Document::selectedItemsChanged, this, onSelectionChange);
    connect(model->document().get(), &Document::currentItemChanged, this, onCurrentChange);
    onSelectionChange();
    onCurrentChange();

    connect(this, &QItemSelectionModel::selectionChanged, model, [this, model] {
        std::unordered_set<SP<Item>> items;
        for (auto index : selectedIndexes()) {
            items.insert(model->itemForIndex(index));
        }
        model->document()->setSelectedItems(std::move(items));
    });
    connect(this, &QItemSelectionModel::currentChanged, model, [this, model] {
        auto item = model->itemForIndex(currentIndex());
        model->document()->setCurrentItem(item);
    });
}

} // namespace Shapecraft
