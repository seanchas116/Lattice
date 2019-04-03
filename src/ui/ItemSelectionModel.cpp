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
        for (auto item : model->document()->selectedObjects()) {
            auto index = model->indexForItem(item);
            selection.select(index, index);
        }
        select(selection, QItemSelectionModel::ClearAndSelect);
    };
    auto onCurrentChange = [this, model] {
        LATTICE_OPTIONAL_GUARD(currentItem, model->document()->currentObject(), clearCurrentIndex();)
        auto current = model->indexForItem(currentItem);
        select(current, QItemSelectionModel::Current);
    };
    connect(model->document().get(), &Document::Document::selectedObjectsChanged, this, onSelectionChange);
    connect(model->document().get(), &Document::Document::currentObjectChanged, this, onCurrentChange);
    onSelectionChange();
    onCurrentChange();

    connect(this, &QItemSelectionModel::selectionChanged, model, [this, model] {
        std::unordered_set<SP<Document::Object>> items;
        for (auto index : selectedIndexes()) {
            items.insert(model->itemForIndex(index));
        }
        model->document()->setSelectedObjects(std::move(items));
    });
    connect(this, &QItemSelectionModel::currentChanged, model, [this, model] {
        auto index = currentIndex();
        if (index.isValid()) {
            auto item = model->itemForIndex(currentIndex());
            model->document()->setCurrentObject(item);
        } else {
            model->document()->setCurrentObject({});
        }
    });
}

}
} // namespace Shapecraft
