#pragma once
#include "../document/MeshObject.hpp"
#include <QObject>

namespace Lattice {
namespace State {

class MeshEditState : public QObject {
    Q_OBJECT
public:
    auto& targetObject() const { return _targetObject; }
    void setTargetObject(Opt<SP<Document::MeshObject>> target);

signals:
    void targetObjectChanged(const Opt<SP<Document::MeshObject>>& target);

private:
    Opt<SP<Document::MeshObject>> _targetObject;
};

} // namespace State
} // namespace Lattice

