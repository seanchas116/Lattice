#pragma once

#include <QAbstractItemModel>
#include <QUuid>
#include "../support/Shorthands.hpp"

namespace Lattice {

namespace Document {
class Document;
class Object;
}

namespace UI {

class ObjectItemModel final : public QAbstractItemModel {
    Q_OBJECT
public:
    ObjectItemModel(const SP<Document::Document>& document, QObject* parent = nullptr);

    const SP<Document::Document>& document() const { return _document; }

    QModelIndex indexForObject(const SP<Document::Object>& object) const;
    SP<Document::Object> objectForIndex(const QModelIndex& index) const;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parentIndex) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    Qt::DropActions supportedDropActions() const override;
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

private:
    void connectObject(const SP<Document::Object>& object);
    void disconnectObject(const SP<Document::Object>& object);

    SP<Document::Document> _document;
    QUuid _uuid;
};

}
} // namespace Shapecraft
