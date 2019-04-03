#pragma once
#include <QItemSelectionModel>
#include "../support/Shorthands.hpp"

namespace Lattice {
namespace UI {

class ObjectItemModel;

class ItemSelectionModel final : public QItemSelectionModel {
    Q_OBJECT
public:
    explicit ItemSelectionModel(ObjectItemModel* model, QObject *parent = nullptr);

signals:

public slots:
};

}
} // namespace Shapecraft
