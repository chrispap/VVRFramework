#include <vvr/glwidget.h>
#include <vvr/scene.h>
#include <iostream>
#include <QtOpenGL>
#include <QMouseEvent>
#include <QTimer>
#include <cmath>

#define ANIM_INTERVAL 10

vvr::GlWidget::GlWidget(vvr::Scene *scene, QWidget *parent) : QGLWidget(parent)
{
    mScene = scene;
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(idle()));
    mTimer.start(ANIM_INTERVAL);
}

vvr::GlWidget::~GlWidget()
{
    delete mScene;
}

void vvr::GlWidget::initializeGL()
{
    mScene->glInit();
}

void vvr::GlWidget::paintGL()
{
    mScene->glRender();
}

void vvr::GlWidget::resizeGL(int width, int height)
{
    mScene->glResize(width, height);
}

void vvr::GlWidget::idle()
{
    if (!mScene->idle())
        mTimer.stop();
    else if (!mTimer.isActive())
        mTimer.start(ANIM_INTERVAL);
    update();
}

void vvr::GlWidget::mousePressEvent(QMouseEvent *event)
{
    setFocus();
    int x = event->x();
    int y = event->y();
    mScene->mouse2pix(x,y);
    mScene->mousePressed(x, y, mkModif(event));
    idle();
}

void vvr::GlWidget::mouseReleaseEvent(QMouseEvent *event)
{
    setFocus();
    int x = event->x();
    int y = event->y();
    mScene->mouse2pix(x,y);
    mScene->mouseReleased(x, y, mkModif(event));
    idle();
}

void vvr::GlWidget::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
    mScene->mouse2pix(x,y);
    mScene->mouseMoved(x, y, mkModif(event));
    update();
}

void vvr::GlWidget::wheelEvent(QWheelEvent *event)
{
    mScene->mouseWheel(event->delta()>0?1:-1, mkModif(event));
    idle();
}

void vvr::GlWidget::keyPressEvent(QKeyEvent *event)
{
    onKeyPressed(event);
}

void vvr::GlWidget::onKeyPressed(QKeyEvent *event)
{
    int modif = mkModif(event);
    QString txt = event->text();
    if (event->key() == Qt::Key_Escape) QApplication::quit();
    else if (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z) mScene->keyEvent(tolower(event->key()), false, modif);
    else if (txt.length()>0) mScene->keyEvent(txt.toStdString()[0],false, modif);
    else if (event->key() == Qt::Key_Left) mScene->arrowEvent(vvr::LEFT, modif);
    else if (event->key() == Qt::Key_Right) mScene->arrowEvent(vvr::RIGHT, modif);
    else if (event->key() == Qt::Key_Up) mScene->arrowEvent(vvr::UP, modif);
    else if (event->key() == Qt::Key_Down) mScene->arrowEvent(vvr::DOWN, modif);
    idle();
}

int vvr::GlWidget::mkModif(QInputEvent *event)
{
    int ctrl  = event->modifiers() & Qt::ControlModifier ? 1 : 0;
    int shift = event->modifiers() & Qt::ShiftModifier   ? 1 : 0;
    int alt   = event->modifiers() & Qt::AltModifier     ? 1 : 0;
    int modif = (ctrl << 0) | (shift << 1) | (alt << 2) ;
    return modif;
}
