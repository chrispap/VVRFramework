#include <vvr/glwidget.h>
#include <vvr/scene.h>
#include <cmath>
#include <iostream>
#include <QApplication>
#include <QOpenGLContext>
#include <QMouseEvent>
#include <QTimer>
#include <gl.h>

#if 0
#define PRINT_TIME_FROM_FUNCTION(msg) \
    std::cout << (getSeconds()*1000)  << "msec - " << \
    __FUNCTION__ <<  \
    " ("<< msg << ")\n";
#else
#define PRINT_TIME_FROM_FUNCTION(msg) (void)(0)
#endif

static int s_mouse_x;
static int s_mouse_y;
static QWidget *s_widget_ptr;

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
    x = s_mouse_x;
    y = s_mouse_y;
    QPoint p = s_widget_ptr->mapFromGlobal(QCursor::pos());
    x = p.x();
    y = p.y();
}

/*--------------------------------------------------------------------------------------*/
vvr::GlWidget::GlWidget(vvr::Scene *scene, QWidget *parent) : QOpenGLWidget(parent)
{
#if 0
    QSurfaceFormat format;
    format.setVersion(4,3);
    format.setSamples(4);
    format.setProfile(QSurfaceFormat::NoProfile);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setSwapInterval(0);
    setFormat(format);
#endif
    m_scene = scene;
    m_timer.setSingleShot(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(idle()));
    s_widget_ptr = this;
    setMouseTracking(true);
    m_timer.start(100);
}

vvr::GlWidget::~GlWidget()
{
    delete m_scene;
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
    PRINT_TIME_FROM_FUNCTION("In");
    m_scene->glRender();
    PRINT_TIME_FROM_FUNCTION("Out");
}

void vvr::GlWidget::resizeGL(int width, int height)
{
    m_scene->glResize(width, height);
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
        int x = event->x();
        int y = event->y();
        m_scene->mouse2pix(x, y);
        m_scene->mousePressed(x, y, make_modifier_flag(event));
        setFocus();
        idle();
    }
}

void vvr::GlWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        int x = event->x();
        int y = event->y();
        m_scene->mouse2pix(x, y);
        m_scene->mouseReleased(x, y, make_modifier_flag(event));
        setFocus();
        idle();
    }
}

void vvr::GlWidget::mouseMoveEvent(QMouseEvent *event)
{
    PRINT_TIME_FROM_FUNCTION("In");
    int x = event->x();
    int y = event->y();
    m_scene->mouse2pix(x, y);

    if (event->buttons() & Qt::LeftButton) {
        m_scene->mouseMoved(x, y, make_modifier_flag(event));
    } else if (event->buttons() == Qt::NoButton) {
        m_scene->mouseHovered(x, y, make_modifier_flag(event));
    } else {
        event->ignore();
        return;
    }

    update();
    event->accept();
    PRINT_TIME_FROM_FUNCTION("Out");
}

void vvr::GlWidget::wheelEvent(QWheelEvent *event)
{
    m_scene->mouseWheel(event->delta()>0?1:-1, make_modifier_flag(event));
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
