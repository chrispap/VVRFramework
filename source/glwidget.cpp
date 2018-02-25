#include <vvr/glwidget.h>
#include <vvr/scene.h>
#include <iostream>
#include <QApplication>
#include <QMouseEvent>
#include <QTimer>
#include <cmath>

static int s_mouse_x;
static int s_mouse_y;
static QWidget *s_widget_ptr;

void vvr::get_mouse_xy(int &x, int &y)
{
    x = s_mouse_x;
    y = s_mouse_y;
    QPoint p = s_widget_ptr->mapFromGlobal(QCursor::pos());
    x = p.x();
    y = p.y();
}

/*--------------------------------------------------------------------------------------*/
vvr::GlWidget::GlWidget(vvr::Scene *scene, QWidget *parent) : QOpenGLWidget(parent)
{
    mScene = scene;
    mTimer.setSingleShot(true);
    connect(&mTimer, SIGNAL(timeout()), this, SLOT(idle()));
    mTimer.start(0);
    setMouseTracking(true);
    s_widget_ptr = this;
}

vvr::GlWidget::~GlWidget()
{
    delete mScene;
}

void vvr::GlWidget::setScene(vvr::Scene *scene)
{
    mScene = scene;
    resizeGL(width(), height());
    idle();
}

/*---[OpenGL]---------------------------------------------------------------------------*/
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

/*---[Events]---------------------------------------------------------------------------*/
void vvr::GlWidget::idle()
{
    if (!mScene->idle()) {
        mTimer.stop();
    } else if (!mTimer.isActive()) {
        mTimer.start(0);
    }
    update();
}

void vvr::GlWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        int x = event->x();
        int y = event->y();
        mScene->mouse2pix(x, y);
        mScene->mousePressed(x, y, mkModif(event));
        setFocus();
        idle();
    }
}

void vvr::GlWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        int x = event->x();
        int y = event->y();
        mScene->mouse2pix(x, y);
        mScene->mouseReleased(x, y, mkModif(event));
        setFocus();
        idle();
    }
}

void vvr::GlWidget::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->x();
    int y = event->y();
    mScene->mouse2pix(x, y);

    if (event->buttons() & Qt::LeftButton) {
        mScene->mouseMoved(x, y, mkModif(event));
        event->accept();
    }
    else if (event->buttons() == Qt::NoButton) {
        mScene->mouseHovered(x, y, mkModif(event));
        event->accept();
    }
    else return;

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
/*--------------------------------------------------------------------------------------*/
