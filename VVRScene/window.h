#ifndef VVR_WINDOW_H
#define VVR_WINDOW_H

#include "scene.h"
#include "ui_window.h"
#include "glwidget.h"

namespace vvr {

class Window : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    Window(vvr::Scene *scene);

private slots:
    void about();

signals:
    void keyPressed(QKeyEvent *event);

protected:
     virtual void keyPressEvent(QKeyEvent* event);

private:
    void createActions();
    void createMenus();

private:
    static QString aboutMessage;
    QMenu *fileMenu, *helpMenu;
    QAction *exitAct, *aboutAct;
    GLWidget *glWidget;
    Scene *scene;
};

}

#endif // VVR_WINDOW_H
