#pragma once
#include "../support/Shorthands.hpp"
#include <QObject>
#include <vector>

class QUndoCommand;
class QUndoStack;

namespace Lattice {

class Change;

namespace Document {

class History final : public QObject {
    Q_OBJECT
  public:
    History();

    void beginChange(const QString &title);
    void addChange(const SP<Change> &change);
    void clear();

    const QUndoStack *undoStack() const { return _undoStack; }

  private:
    QUndoStack *_undoStack;
    int _currentID = 0;
    QString _currentTitle;
};

} // namespace Document
} // namespace Lattice
