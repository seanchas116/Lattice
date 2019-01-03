#include "History.hpp"
#include <QUndoStack>
#include <QUndoCommand>

namespace Lattice::Document {

namespace {

class ChangeCommand : public QUndoCommand {
public:
    ChangeCommand(int id, const QString& text, SP<Change> change) : _id(id), _change(change) {
        setText(text);
    }

    void redo() override {
        _change->redo();
    }

    void undo() override {
        _change->undo();
    }

    int id() const override {
        return _id;
    }

    bool mergeWith(const QUndoCommand *other) override {
        auto otherChangeCommand = dynamic_cast<const ChangeCommand*>(other);
        if (!otherChangeCommand) {
            return false;
        }
        return _change->mergeWith(otherChangeCommand->_change);
    }

private:
    int _id;
    SP<Change> _change;
};

}

Change::~Change() = default;

bool Change::mergeWith(const SP<const Change> &other) {
    Q_UNUSED(other)
    return false;
}

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

} // namespace Lattice
