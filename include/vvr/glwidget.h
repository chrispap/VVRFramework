#ifndef VVR_GLWIDGET_H
#define VVR_GLWIDGET_H

#include "vvrframework_DLL.h"
#include "scene.h"
#include <QOpenGLWidget>
#include <QInputEvent>
#include <QTimer>

namespace vvr {

class VVRFramework_API GlWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    GlWidget(vvr::Scene *scene=0, QWidget *parent = 0);

public slots:
    void idle();
    void setScene(vvr::Scene *scene);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
    bool eventFilter(QObject*, QEvent*) override;

private:
    Scene *m_scene;
    QTimer m_timer;
};

}

#endif
