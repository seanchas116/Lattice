#pragma once
#include <QObject>

namespace Lattice {
namespace Document {

class Material : public QObject {
    Q_OBJECT
public:
    explicit Material(QObject *parent = nullptr);
};

} // namespace Document
} // namespace Lattice
