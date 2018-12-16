#pragma once
#include <QItemSelectionModel>
#include "../support/Pointer.hpp"

namespace Lattice {

class ItemModel;

class ItemSelectionModel : public QItemSelectionModel
{
    Q_OBJECT
public:
    explicit ItemSelectionModel(ItemModel* model, QObject *parent = nullptr);

signals:

public slots:
};

} // namespace Shapecraft
