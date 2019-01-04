#pragma once
#include <QObject>

namespace Lattice {

class ItemPicker : public QObject {
    Q_OBJECT
public:
    explicit ItemPicker(QObject *parent = nullptr);

signals:

public slots:
};

} // namespace Lattice
