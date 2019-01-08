#pragma once
#include <QObject>
#include <vector>
#include "../support/Pointer.hpp"

class QUndoCommand;
class QUndoStack;

namespace Lattice::Document {

class Change {
    Q_DISABLE_COPY(Change)
public:
    Change() = default;
    virtual ~Change();
    virtual void redo() = 0;
    virtual void undo() = 0;
    virtual bool mergeWith(const SP<const Change>& other);
};

class History final : public QObject {
    Q_OBJECT
public:
    History();

    void beginChange(const QString& title);
    void addChange(const SP<Change>& change);
    void clear();

    const QUndoStack* undoStack() const { return _undoStack; }

private:
    QUndoStack* _undoStack;
    int _currentID = 0;
    QString _currentTitle;
};

} // namespace Lattice
