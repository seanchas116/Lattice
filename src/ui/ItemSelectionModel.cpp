#include "ItemSelectionModel.hpp"
#include "ObjectItemModel.hpp"
#include "../document/Document.hpp"
#include "../support/OptionalGuard.hpp"
#include <QtDebug>

namespace Lattice {
namespace UI {

ItemSelectionModel::ItemSelectionModel(ObjectItemModel *model, QObject *parent) : QItemSelectionModel(model, parent)
{
    auto onSelectionChange = [this, model] {
        QItemSelection selection;
        for (auto item : model->document()->selectedObjects()) {
            auto index = model->indexForObject(item);
            selection.select(index, index);
        }
        select(selection, QItemSelectionModel::ClearAndSelect);
    };
    auto onCurrentChange = [this, model] {
        LATTICE_OPTIONAL_GUARD(currentItem, model->document()->currentObject(), clearCurrentIndex();)
        auto current = model->indexForObject(currentItem);
        select(current, QItemSelectionModel::Current);
    };
    connect(model->document().get(), &Document::Document::selectedObjectsChanged, this, onSelectionChange);
    connect(model->document().get(), &Document::Document::currentObjectChanged, this, onCurrentChange);
    onSelectionChange();
    onCurrentChange();

    connect(this, &QItemSelectionModel::selectionChanged, model, [this, model] {
        std::unordered_set<SP<Document::Object>> items;
        for (auto index : selectedIndexes()) {
            items.insert(model->objectForIndex(index));
        }
        model->document()->setSelectedObjects(std::move(items));
    });
    connect(this, &QItemSelectionModel::currentChanged, model, [this, model] {
        auto index = currentIndex();
        if (index.isValid()) {
            auto item = model->objectForIndex(currentIndex());
            model->document()->setCurrentObject(item);
        } else {
            model->document()->setCurrentObject({});
        }
    });
}

}
} // namespace Shapecraft
