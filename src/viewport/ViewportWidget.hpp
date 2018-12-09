#pragma once
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <memory>

namespace Lattice {

class ViewportRenderer;

class ViewportWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    explicit ViewportWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    std::shared_ptr<ViewportRenderer> _renderer;
};

} // namespace Lattice

