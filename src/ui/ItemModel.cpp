#include "ItemModel.hpp"
#include "../document/Item.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../support/OptionalGuard.hpp"
#include <QMimeData>
#include <QtDebug>
#include <QDataStream>

namespace Lattice {
namespace UI {

ItemModel::ItemModel(const SP<Document::Document> &document, QObject *parent) :
    QAbstractItemModel(parent),
    _document(document),
    _uuid(QUuid::createUuid())
{
    connectItem(document->rootItem());
}

QVariant ItemModel::data(const QModelIndex &index, int role) const {
    auto item = itemForIndex(index);
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return QString::fromStdString(item->name());
    default:
        return QVariant();
    }
}

bool ItemModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    auto item = itemForIndex(index);
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        _document->history()->beginChange(tr("Set Item Name"));
        item->setName(value.toString().toStdString());
        return true;
    default:
        return false;
    }
}

Qt::ItemFlags ItemModel::flags(const QModelIndex &index) const {
    Q_UNUSED(index)
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

QModelIndex ItemModel::index(int row, int column, const QModelIndex &parentIndex) const {
    Q_UNUSED(column)
    auto parent = itemForIndex(parentIndex);
    return indexForItem(parent->childItems()[row]);
}

QModelIndex ItemModel::parent(const QModelIndex &child) const {
    auto item = itemForIndex(child);
    LATTICE_OPTIONAL_GUARD(parent, item->parentItem(), return {};)
    return indexForItem(parent);
}

int ItemModel::rowCount(const QModelIndex &parent) const {
    return int(itemForIndex(parent)->childItems().size());
}

int ItemModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return 1;
}

Qt::DropActions ItemModel::supportedDropActions() const {
    return Qt::MoveAction | Qt::CopyAction;
}

namespace {
const QString _itemIndexMimeType = "application/x-shapecraft-item-index";
}

QStringList ItemModel::mimeTypes() const {
    return {_itemIndexMimeType};
}

QMimeData *ItemModel::mimeData(const QModelIndexList &indexes) const {
    auto mimeData = new QMimeData();

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << _uuid;
    for (const auto& index : indexes) {
        auto item = itemForIndex(index);
        stream << QVector<int>::fromStdVector(item->indexPath());
    }
    mimeData->setData(_itemIndexMimeType, data);

    return mimeData;
}

bool ItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    if (column != 0) {
        return false;
    }
    if (action == Qt::IgnoreAction) {
        return true;
    }

    auto parentItem = itemForIndex(parent);

    if (data->hasFormat(_itemIndexMimeType)) {
        QByteArray encodedData = data->data(_itemIndexMimeType);
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        QUuid uuid;
        stream >> uuid;
        if (uuid != _uuid) {
            return false;
        }

        std::vector<SP<Document::Item>> items;

        while (!stream.atEnd()) {
            QVector<int> indexPath;
            stream >> indexPath;

            SP<Document::Item> item = _document->rootItem();
            for (int index : indexPath) {
                item = item->childItems()[index];
            }

            items.push_back(item);
        }

        Opt<SP<Document::Item>> ref;
        if (row != -1 && row != int(parentItem->childItems().size())) {
            ref = parentItem->childItems()[row];
        }
        switch (action) {
        default:
        case Qt::CopyAction:
            for (const auto& item : items) {
                parentItem->insertItemBefore(item->clone(), ref);
            }
            break;
        case Qt::MoveAction:
            for (const auto& item : items) {
                parentItem->insertItemBefore(item, ref);
            }
            break;
        }

        return true;
    }
    return false;
}

QModelIndex ItemModel::indexForItem(const SP<Document::Item>& item) const {
    if (item == _document->rootItem()) {
        return {};
    }
    return createIndex(item->index(), 0, item.get());
}

SP<Document::Item> ItemModel::itemForIndex(const QModelIndex &index) const {
    if (!index.isValid()) {
        return _document->rootItem();
    }
    return static_cast<Document::Item *>(index.internalPointer())->sharedFromThis();
}

void ItemModel::connectItem(const SP<Document::Item> &item) {
    auto itemPtr = item.get();
    connect(itemPtr, &Document::Item::nameChanged, this, [this, itemPtr] () {
        auto index = indexForItem(itemPtr->sharedFromThis());
        emit dataChanged(index, index, {Qt::DisplayRole});
    });
    connect(itemPtr, &Document::Item::childItemsAboutToBeInserted, this, [this, itemPtr] (int first, int last) {
        auto index = indexForItem(itemPtr->sharedFromThis());
        beginInsertRows(index, first, last);
    });
    connect(itemPtr, &Document::Item::childItemsInserted, this, [this, itemPtr] (int first, int last) {
        auto item = itemPtr->sharedFromThis();
        for (int i = first; i <= last; ++i) {
            auto& child = item->childItems()[i];
            connectItem(child);
        }
        endInsertRows();
    });
    connect(itemPtr, &Document::Item::childItemsAboutToBeRemoved, this, [this, itemPtr] (int first, int last) {
        auto item = itemPtr->sharedFromThis();
        for (int i = first; i <= last; ++i) {
            auto& child = item->childItems()[i];
            disconnectItem(child);
        }
        auto index = indexForItem(item);
        beginRemoveRows(index, first, last);
    });
    connect(itemPtr, &Document::Item::childItemsRemoved, this, [this] () {
        endRemoveRows();
    });

    for (auto& child : item->childItems()) {
        connectItem(child);
    }
}

void ItemModel::disconnectItem(const SP<Document::Item> &item) {
    disconnect(item.get(), nullptr, this, nullptr);
}

}
}
