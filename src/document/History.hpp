#pragma once
#include <QObject>
#include <vector>
#include "../support/Pointer.hpp"

class QUndoCommand;
class QUndoStack;

namespace Lattice::Document {

class Change;

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
