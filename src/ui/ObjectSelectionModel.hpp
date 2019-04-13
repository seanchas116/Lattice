#pragma once
#include <QItemSelectionModel>
#include "../support/Shorthands.hpp"

namespace Lattice {
namespace UI {

class ObjectItemModel;

class ObjectSelectionModel final : public QItemSelectionModel {
    Q_OBJECT
public:
    explicit ObjectSelectionModel(ObjectItemModel* model, QObject *parent = nullptr);

signals:

public slots:
};

}
} // namespace Shapecraft
