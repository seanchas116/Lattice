#include "ItemSelectionModel.hpp"
#include "ItemModel.hpp"
#include "../document/Document.hpp"
#include "../support/OptionalGuard.hpp"
#include <QtDebug>

namespace Lattice {
namespace UI {

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
        LATTICE_OPTIONAL_GUARD(currentItem, model->document()->currentItem(), clearCurrentIndex();)
        auto current = model->indexForItem(currentItem);
        select(current, QItemSelectionModel::Current);
    };
    connect(model->document().get(), &Document::Document::selectedItemsChanged, this, onSelectionChange);
    connect(model->document().get(), &Document::Document::currentItemChanged, this, onCurrentChange);
    onSelectionChange();
    onCurrentChange();

    connect(this, &QItemSelectionModel::selectionChanged, model, [this, model] {
        std::unordered_set<SP<Document::Item>> items;
        for (auto index : selectedIndexes()) {
            items.insert(model->itemForIndex(index));
        }
        model->document()->setSelectedItems(std::move(items));
    });
    connect(this, &QItemSelectionModel::currentChanged, model, [this, model] {
        auto index = currentIndex();
        if (index.isValid()) {
            auto item = model->itemForIndex(currentIndex());
            model->document()->setCurrentItem(item);
        } else {
            model->document()->setCurrentItem({});
        }
    });
}

}
} // namespace Shapecraft
