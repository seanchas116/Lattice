#pragma once
#include <QtGlobal>

namespace Lattice::OldGL {

template <typename T>
class Binder final {
    Q_DISABLE_COPY(Binder)
public:
    Binder(T& resource) : _resource(resource) {
        _resource.bind();
    }
    ~Binder() {
        _resource.unbind();
    }
private:
    T& _resource;
};

}
