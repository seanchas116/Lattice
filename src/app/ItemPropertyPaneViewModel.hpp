#pragma once
#include <QObject>

namespace Lattice {

class ItemPropertyPaneViewModel : public QObject {
    Q_OBJECT
public:
    explicit ItemPropertyPaneViewModel(QObject *parent = nullptr);

    float positionX() const { return _positionX; }
    void setPositionX(float positionX);

    float positionY() const { return _positionY; }
    void setPositionY(float positionY);

    float positionZ() const { return _positionZ; }
    void setPositionZ(float positionZ);

    bool hasPositionX() const { return _hasPositionX; }
    void setHasPositionX(bool hasPositionX);

    bool hasPositionY() const { return _hasPositionY; }
    void setHasPositionY(bool hasPositionY);

    bool hasPositionZ() const { return _hasPositionZ; }
    void setHasPositionZ(bool hasPositionZ);

signals:
    void positionXChanged(float positionX);
    void positionYChanged(float positionY);
    void positionZChanged(float positionZ);
    void hasPositionXChanged(bool hasPositionX);
    void hasPositionYChanged(bool hasPositionY);
    void hasPositionZChanged(bool hasPositionZ);

private:
    float _positionX;
    float _positionY;
    float _positionZ;
    bool _hasPositionX;
    bool _hasPositionY;
    bool _hasPositionZ;
};

} // namespace Lattice
