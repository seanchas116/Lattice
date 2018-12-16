#pragma once

#include <QObject>
#include "../support/Pointer.hpp"

namespace Lattice {

class Document;

class AppState : public QObject {
    Q_OBJECT
public:
    AppState();

    const auto& document() const { return _document; }

private:
    SP<Document> _document;
};

} // namespace Lattice
