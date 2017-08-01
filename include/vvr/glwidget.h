#ifndef VVR_GLWIDGET_H
#define VVR_GLWIDGET_H

#include "vvrframework_DLL.h"
#include "scene.h"
#include <QtOpenGL>
#include <QGLWidget>
#include <QInputEvent>

namespace vvr {

class vvrframework_API GlWidget : public QGLWidget
{
    Q_OBJECT

public:
    GlWidget(vvr::Scene *scene, QWidget *parent = 0);
    ~GlWidget();

    public slots:
    void onKeyPressed(QKeyEvent *event);
    void idle();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    static int mkModif(QInputEvent *event);

    Scene *mScene;
    QTimer mTimer;
};

}

#endif
