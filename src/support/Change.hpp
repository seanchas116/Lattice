#pragma once
#include "Shorthands.hpp"
#include <QtGlobal>

namespace Lattice {

class Change {
    Q_DISABLE_COPY(Change)
  public:
    Change() = default;
    virtual ~Change();
    virtual void apply() = 0;
    virtual SP<Change> invert() const = 0;
    virtual bool mergeWith(const SP<const Change> &other);
};

} // namespace Lattice
