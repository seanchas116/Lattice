#pragma once
#include <QItemSelectionModel>
#include "../support/Pointer.hpp"

namespace Lattice::UI {

class ItemModel;

class ItemSelectionModel final : public QItemSelectionModel {
    Q_OBJECT
public:
    explicit ItemSelectionModel(ItemModel* model, QObject *parent = nullptr);

signals:

public slots:
};

} // namespace Shapecraft
