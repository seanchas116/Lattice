#pragma once
#include "../support/Shorthands.hpp"
#include <QItemSelectionModel>

namespace Lattice {
namespace UI {

class ObjectItemModel;

class ObjectSelectionModel final : public QItemSelectionModel {
    Q_OBJECT
  public:
    explicit ObjectSelectionModel(ObjectItemModel *model, QObject *parent = nullptr);

  signals:

  public slots:
};

} // namespace UI
} // namespace Lattice
