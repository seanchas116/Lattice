#include "ObjectSelectionModel.hpp"
#include "ObjectItemModel.hpp"
#include "../document/Document.hpp"
#include "../support/OptionalGuard.hpp"
#include <QtDebug>

namespace Lattice {
namespace UI {

ObjectSelectionModel::ObjectSelectionModel(ObjectItemModel *model, QObject *parent) : QItemSelectionModel(model, parent)
{
    auto onSelectionChange = [this, model] {
        QItemSelection selection;
        for (auto object : model->document()->selectedObjects()) {
            auto index = model->indexForObject(object);
            selection.select(index, index);
        }
        select(selection, QItemSelectionModel::ClearAndSelect);
    };
    auto onCurrentChange = [this, model] {
        LATTICE_OPTIONAL_GUARD(currentObject, model->document()->currentObject(), clearCurrentIndex();)
        auto current = model->indexForObject(currentObject);
        select(current, QItemSelectionModel::Current);
    };
    connect(model->document().get(), &Document::Document::selectedObjectsChanged, this, onSelectionChange);
    connect(model->document().get(), &Document::Document::currentObjectChanged, this, onCurrentChange);
    onSelectionChange();
    onCurrentChange();

    connect(this, &QItemSelectionModel::selectionChanged, model, [this, model] {
        std::unordered_set<SP<Document::Object>> objects;
        for (auto index : selectedIndexes()) {
            objects.insert(model->objectForIndex(index));
        }
        model->document()->setSelectedObjects(std::move(objects));
    });
    connect(this, &QItemSelectionModel::currentChanged, model, [this, model] {
        auto index = currentIndex();
        if (index.isValid()) {
            auto object = model->objectForIndex(currentIndex());
            model->document()->setCurrentObject(object);
        } else {
            model->document()->setCurrentObject({});
        }
    });
}

}
} // namespace Shapecraft
