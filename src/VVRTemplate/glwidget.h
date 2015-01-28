#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL>
#include <QGLWidget>
#include <QInputEvent>
#include <scene.h>

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(vvr::Scene *scene, QWidget *parent = 0);
    ~GLWidget();

public slots:
    void onKeyPressed(QKeyEvent *event);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void idle();

private: //data
    vvr::Scene *mScene;
    QTimer timer;

private:
    static int mkModif(QInputEvent *event);

};

#endif // GLWIDGET_H
