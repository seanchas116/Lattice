#pragma once
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>

namespace Lattice {

class ViewportWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions {
    Q_OBJECT
public:
    explicit ViewportWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};

} // namespace Lattice

