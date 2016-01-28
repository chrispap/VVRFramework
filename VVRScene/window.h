#ifndef VVR_WINDOW_H
#define VVR_WINDOW_H

#include "scene.h"
#include "ui_window.h"
#include "glwidget.h"
#include "stdredirector.h"

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
    StdRedirector<> *m_std_cout_logger;
    StdRedirector<> *m_std_cerr_logger;

private:
    static void log_cout(const char* ptr, std::streamsize count, void*);
    static void log_cerr(const char* ptr, std::streamsize count, void*);
};

}

#endif // VVR_WINDOW_H
