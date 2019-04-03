#include "ObjectItemModel.hpp"
#include "../document/Object.hpp"
#include "../document/Document.hpp"
#include "../document/History.hpp"
#include "../support/OptionalGuard.hpp"
#include <QMimeData>
#include <QtDebug>
#include <QDataStream>

namespace Lattice {
namespace UI {

ObjectItemModel::ObjectItemModel(const SP<Document::Document> &document, QObject *parent) :
    QAbstractItemModel(parent),
    _document(document),
    _uuid(QUuid::createUuid())
{
    connectObject(document->rootObject());
}

QVariant ObjectItemModel::data(const QModelIndex &index, int role) const {
    auto object = objectForIndex(index);
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return QString::fromStdString(object->name());
    default:
        return QVariant();
    }
}

bool ObjectItemModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    auto object = objectForIndex(index);
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        _document->history()->beginChange(tr("Set Object Name"));
        object->setName(value.toString().toStdString());
        return true;
    default:
        return false;
    }
}

Qt::ItemFlags ObjectItemModel::flags(const QModelIndex &index) const {
    Q_UNUSED(index)
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

QModelIndex ObjectItemModel::index(int row, int column, const QModelIndex &parentIndex) const {
    Q_UNUSED(column)
    auto parent = objectForIndex(parentIndex);
    return indexForObject(parent->childObjects()[row]);
}

QModelIndex ObjectItemModel::parent(const QModelIndex &child) const {
    auto object = objectForIndex(child);
    LATTICE_OPTIONAL_GUARD(parent, object->parentObject(), return {};)
    return indexForObject(parent);
}

int ObjectItemModel::rowCount(const QModelIndex &parent) const {
    return int(objectForIndex(parent)->childObjects().size());
}

int ObjectItemModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return 1;
}

Qt::DropActions ObjectItemModel::supportedDropActions() const {
    return Qt::MoveAction | Qt::CopyAction;
}

namespace {
const QString _indexMimeType = "application/x-shapecraft-object-index";
}

QStringList ObjectItemModel::mimeTypes() const {
    return {_indexMimeType};
}

QMimeData *ObjectItemModel::mimeData(const QModelIndexList &indexes) const {
    auto mimeData = new QMimeData();

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << _uuid;
    for (const auto& index : indexes) {
        auto object = objectForIndex(index);
        stream << QVector<int>::fromStdVector(object->indexPath());
    }
    mimeData->setData(_indexMimeType, data);

    return mimeData;
}

bool ObjectItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
    if (column != 0) {
        return false;
    }
    if (action == Qt::IgnoreAction) {
        return true;
    }

    auto parentObject = objectForIndex(parent);

    if (data->hasFormat(_indexMimeType)) {
        QByteArray encodedData = data->data(_indexMimeType);
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        QUuid uuid;
        stream >> uuid;
        if (uuid != _uuid) {
            return false;
        }

        std::vector<SP<Document::Object>> objects;

        while (!stream.atEnd()) {
            QVector<int> indexPath;
            stream >> indexPath;

            SP<Document::Object> object = _document->rootObject();
            for (int index : indexPath) {
                object = object->childObjects()[index];
            }

            objects.push_back(object);
        }

        Opt<SP<Document::Object>> ref;
        if (row != -1 && row != int(parentObject->childObjects().size())) {
            ref = parentObject->childObjects()[row];
        }
        switch (action) {
        default:
        case Qt::CopyAction:
            for (const auto& object : objects) {
                parentObject->insertObjectBefore(object->clone(), ref);
            }
            break;
        case Qt::MoveAction:
            for (const auto& object : objects) {
                parentObject->insertObjectBefore(object, ref);
            }
            break;
        }

        return true;
    }
    return false;
}

QModelIndex ObjectItemModel::indexForObject(const SP<Document::Object>& object) const {
    if (object == _document->rootObject()) {
        return {};
    }
    return createIndex(object->index(), 0, object.get());
}

SP<Document::Object> ObjectItemModel::objectForIndex(const QModelIndex &index) const {
    if (!index.isValid()) {
        return _document->rootObject();
    }
    return static_cast<Document::Object *>(index.internalPointer())->sharedFromThis();
}

void ObjectItemModel::connectObject(const SP<Document::Object> &object) {
    auto objectPtr = object.get();
    connect(objectPtr, &Document::Object::nameChanged, this, [this, objectPtr] () {
        auto index = indexForObject(objectPtr->sharedFromThis());
        emit dataChanged(index, index, {Qt::DisplayRole});
    });
    connect(objectPtr, &Document::Object::childObjectsAboutToBeInserted, this, [this, objectPtr] (int first, int last) {
        auto index = indexForObject(objectPtr->sharedFromThis());
        beginInsertRows(index, first, last);
    });
    connect(objectPtr, &Document::Object::childObjectsInserted, this, [this, objectPtr] (int first, int last) {
        auto object = objectPtr->sharedFromThis();
        for (int i = first; i <= last; ++i) {
            auto& child = object->childObjects()[i];
            connectObject(child);
        }
        endInsertRows();
    });
    connect(objectPtr, &Document::Object::childObjectsAboutToBeRemoved, this, [this, objectPtr] (int first, int last) {
        auto object = objectPtr->sharedFromThis();
        for (int i = first; i <= last; ++i) {
            auto& child = object->childObjects()[i];
            disconnectObject(child);
        }
        auto index = indexForObject(object);
        beginRemoveRows(index, first, last);
    });
    connect(objectPtr, &Document::Object::childObjectsRemoved, this, [this] () {
        endRemoveRows();
    });

    for (auto& child : object->childObjects()) {
        connectObject(child);
    }
}

void ObjectItemModel::disconnectObject(const SP<Document::Object> &object) {
    disconnect(object.get(), nullptr, this, nullptr);
}

}
}
