#pragma once
#include "Object.hpp"

namespace Lattice {
namespace Document {

class EmptyItem final : public Object {
    Q_OBJECT
public:
    EmptyItem();
    SP<Object> clone() const override;
};

}
} // namespace Lattice
