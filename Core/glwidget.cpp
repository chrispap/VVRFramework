#include <vvr/glwidget.h>
#include <vvr/scene.h>
#include <vvr/macros.h>
#include <cmath>
#include <iostream>
#include <QApplication>
#include <QOpenGLContext>
#include <QMouseEvent>
#include <QTimer>
#include <QtGui> //gl.h

#define DISABLE_PRINTS 1
#if DISABLE_PRINTS
#undef vvr_echo_time_from_function
#define vvr_echo_time_from_function(x) (void)(0)
#endif

static QWidget *s_widget_ptr = NULL;
static float s_dpr = 1.0f;

static int make_modifier_flag(QInputEvent *event)
{
    int modif = 0;
    bool ctrl  = event->modifiers() & Qt::ControlModifier;
    bool shift = event->modifiers() & Qt::ShiftModifier;
    bool alt   = event->modifiers() & Qt::AltModifier;
    modif |= (ctrl  << 0);
    modif |= (shift << 1);
    modif |= (alt   << 2);
    return modif;
}

void vvr::get_mouse_xy(int &x, int &y)
{
    QPoint p = s_widget_ptr->mapFromGlobal(QCursor::pos());
    x = p.x() * s_dpr;
    y = p.y() * s_dpr;
}

/*--------------------------------------------------------------------------------------*/
vvr::GlWidget::GlWidget(vvr::Scene *scene, QWidget *parent) : QOpenGLWidget(parent)
{
    s_widget_ptr = this;
    m_scene = scene;
    m_timer.setSingleShot(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(idle()));
    setMouseTracking(true);
    m_timer.start(100);
}

void vvr::GlWidget::setScene(vvr::Scene *scene)
{
    m_scene = scene;
    resizeGL(width(), height());
    idle();
}

/*---[OpenGL]---------------------------------------------------------------------------*/
void vvr::GlWidget::initializeGL()
{
    m_scene->glInit();
}

void vvr::GlWidget::paintGL()
{
    vvr_echo_time_from_function("In");
    m_scene->glRender();
    vvr_echo_time_from_function("Out");
}

void vvr::GlWidget::resizeGL(int width, int height)
{
    s_dpr = s_widget_ptr->devicePixelRatio();
    vvr_echo(s_dpr);
    m_scene->glResize(width * s_dpr, height * s_dpr);
}

/*---[Events]---------------------------------------------------------------------------*/
void vvr::GlWidget::idle()
{
    if (!m_scene->idle()) {
        m_timer.stop();
    } else if (!m_timer.isActive()) {
        m_timer.start(0);
    }
    update();
}

void vvr::GlWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int x = event->position().x() * s_dpr;
        int y = event->position().y() * s_dpr;
        m_scene->mouse2pix(x, y);
        m_scene->mousePressed(x, y, make_modifier_flag(event));
        setFocus();
        idle();
    }
}

void vvr::GlWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int x = event->position().x() * s_dpr;
        int y = event->position().y() * s_dpr;
        m_scene->mouse2pix(x, y);
        m_scene->mouseReleased(x, y, make_modifier_flag(event));
        setFocus();
        idle();
    }
}

void vvr::GlWidget::mouseMoveEvent(QMouseEvent *event)
{
    vvr_echo_time_from_function("In");
    int x = event->position().x() * s_dpr;
    int y = event->position().y() * s_dpr;
    m_scene->mouse2pix(x, y);

    if (event->buttons() & Qt::LeftButton) {
        m_scene->mouseMoved(x, y, make_modifier_flag(event));
    } else if (event->buttons() == Qt::NoButton) {
        m_scene->mouseHovered(x, y, make_modifier_flag(event));
    } else {
        return event->ignore();
    }

    update();
    event->accept();
    vvr_echo_time_from_function("Out");
}

void vvr::GlWidget::wheelEvent(QWheelEvent *event)
{
    m_scene->mouseWheel(event->angleDelta().y() > 0? 1: -1, make_modifier_flag(event));
    idle();
}

void vvr::GlWidget::keyPressEvent(QKeyEvent *event)
{
    int modif = make_modifier_flag(event);
    QString txt = event->text();
    if (event->key() == Qt::Key_Escape) QApplication::quit();
    else if (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z) m_scene->keyEvent(tolower(event->key()), false, modif);
    else if (txt.length()>0) m_scene->keyEvent(txt.toStdString()[0],false, modif);
    else if (event->key() == Qt::Key_Left) m_scene->arrowEvent(vvr::LEFT, modif);
    else if (event->key() == Qt::Key_Right) m_scene->arrowEvent(vvr::RIGHT, modif);
    else if (event->key() == Qt::Key_Up) m_scene->arrowEvent(vvr::UP, modif);
    else if (event->key() == Qt::Key_Down) m_scene->arrowEvent(vvr::DOWN, modif);
    idle();
}

bool vvr::GlWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        keyPressEvent(keyEvent);
        return true;
    } else return false;
}
/*--------------------------------------------------------------------------------------*/
