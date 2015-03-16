#include "glwidget.h"

#include <iostream>
#include <QtOpenGL>
#include <QMouseEvent>
#include <QTimer>
#include <math.h>
#include "scene.h"

#define ANIM_INTERVAL 10

vvr::GLWidget::GLWidget(vvr::Scene *scene, QWidget *parent) : QGLWidget(parent)
{
     mScene = scene;
     connect(&timer, SIGNAL(timeout()), this, SLOT(idle()));
     timer.start(ANIM_INTERVAL);
}

vvr::GLWidget::~GLWidget()
{
    delete mScene;
}

void vvr::GLWidget::initializeGL()
{
    mScene->GL_Init();
}

void vvr::GLWidget::paintGL()
{
    mScene->GL_Render();
}

void vvr::GLWidget::resizeGL(int width, int height)
{
    mScene->GL_Resize(width, height);
}

void vvr::GLWidget::idle()
{
    if (!mScene->idle())
        timer.stop();
    else if (!timer.isActive())
        timer.start(ANIM_INTERVAL);
    updateGL();
}

void vvr::GLWidget::mousePressEvent(QMouseEvent *event)
{
    setFocus();
    int x = event->x();
    int y = event->y();
    mScene->mouse2pix(x,y);
    mScene->mousePressed(x, y, mkModif(event));
    idle();
}

void vvr::GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
    mScene->mouse2pix(x,y);
    mScene->mouseMoved(x, y, mkModif(event));
    idle();
}

void vvr::GLWidget::wheelEvent(QWheelEvent *event)
{
    mScene->mouseWheel(event->delta()>0?1:-1, mkModif(event));
    idle();
}

void vvr::GLWidget::keyPressEvent(QKeyEvent *event)
{
    onKeyPressed(event);
}

void vvr::GLWidget::onKeyPressed(QKeyEvent *event)
{
    int modif = mkModif(event);
    QString c = event->text();

    if (event->key() == Qt::Key_Escape) QApplication::exit(0);
    else if (c.length()>0) mScene->keyEvent(c.toLatin1()[0],false, modif);
    else if (event->key() == Qt::Key_Left) mScene->arrowEvent(vvr::LEFT, modif);
    else if (event->key() == Qt::Key_Right) mScene->arrowEvent(vvr::RIGHT, modif);
    else if (event->key() == Qt::Key_Up) mScene->arrowEvent(vvr::UP, modif);
    else if (event->key() == Qt::Key_Down) mScene->arrowEvent(vvr::DOWN, modif);

    idle();
}

int vvr::GLWidget::mkModif(QInputEvent *event)
{
    int ctrl  = event->modifiers() & Qt::ControlModifier ? 1 : 0;
    int shift = event->modifiers() & Qt::ShiftModifier   ? 1 : 0;
    int modif = (shift << 1) | (ctrl << 0);
    return modif;
}
