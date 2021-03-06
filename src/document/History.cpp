#include "History.hpp"
#include "../support/Change.hpp"
#include <QUndoCommand>
#include <QUndoStack>

namespace Lattice {
namespace Document {

namespace {

class ChangeCommand : public QUndoCommand {
  public:
    ChangeCommand(int id, const QString &text, const SP<Change> &change) : _id(id), _changes({change}) {
        setText(text);
    }

    void redo() override {
        for (auto &change : _changes) {
            change->apply();
        }
    }

    void undo() override {
        for (int i = _changes.size() - 1; i >= 0; --i) {
            _changes[i]->invert()->apply();
        }
    }

    int id() const override {
        return _id;
    }

    bool mergeWith(const QUndoCommand *other) override {
        auto otherChangeCommand = dynamic_cast<const ChangeCommand *>(other);
        if (!otherChangeCommand) {
            return false;
        }

        for (auto &change : otherChangeCommand->_changes) {
            if (_changes[_changes.size() - 1]->mergeWith(change)) {
                continue;
            }
            _changes.push_back(change);
        }
        return true;
    }

  private:
    int _id;
    std::vector<SP<Change>> _changes;
};

} // namespace

History::History() {
    _undoStack = new QUndoStack(this);
}

void History::beginChange(const QString &title) {
    _currentTitle = title;
    ++_currentID;
}

void History::addChange(const SP<Change> &change) {
    auto command = new ChangeCommand(_currentID, _currentTitle, change);
    _undoStack->push(command);
}

void History::clear() {
    _undoStack->clear();
    _currentID = 0;
    _currentTitle.clear();
}

} // namespace Document
} // namespace Lattice
