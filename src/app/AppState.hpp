#pragma once

#include <QObject>
#include "../support/Pointer.hpp"

namespace Lattice {

class Document;
class Item;
class MeshItem;

class AppState : public QObject {
    Q_OBJECT
public:
    AppState();

    const SP<Item>& currentItem() const { return _currentItem; }
    SP<MeshItem> currentMeshItem() const;

    void setCurrentItem(const SP<Item>& item);

    const auto& document() const { return _document; }

signals:
    void currentItemChanged(const SP<Item>& item);
    void currentMeshItemChanged(const SP<Item>& item);

private:
    SP<Document> _document;
    SP<Item> _currentItem;
};

} // namespace Lattice
