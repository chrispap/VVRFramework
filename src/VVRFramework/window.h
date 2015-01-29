#ifndef VVRWINDOW_H
#define VVRWINDOW_H

#include "scene.h"
#include "ui_window.h"

class GLWidget;

namespace vvr {

class Window : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    Window(vvr::Scene *scene);
    ~Window();

private slots:
    void about();

signals:
    void keyPressed(QKeyEvent *event);

protected:
     virtual void keyPressEvent(QKeyEvent* event);

private: // methods
    void createActions();
    void createMenus();

private: // data
    static QString aboutMessage;
    QMenu *fileMenu, *helpMenu;
    QAction *exitAct, *aboutAct;
    GLWidget *glWidget;
    vvr::Scene *scene;
};

}

#endif
