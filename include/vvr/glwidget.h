#ifndef VVR_GLWIDGET_H
#define VVR_GLWIDGET_H

#include "vvrframework_DLL.h"
#include "scene.h"
#include <QtOpenGL>
#include <QGLWidget>
#include <QInputEvent>

namespace vvr {

class VVRFramework_API GlWidget : public QGLWidget
{
    Q_OBJECT

public:
    GlWidget(vvr::Scene *scene, QWidget *parent = 0);
    ~GlWidget();

public slots:
    void idle();
    void onKeyPressed(QKeyEvent *event);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    static int mkModif(QInputEvent *event);

private:
    Scene *mScene;
    QTimer mTimer;
};

}

#endif
