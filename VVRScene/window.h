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
    void sliderMoved(int val);

signals:
    void keyPressed(QKeyEvent *event);

private:
    virtual void keyPressEvent(QKeyEvent* event);
    void createActions();
    void createMenus();

private:
    static void log_cout(const char* ptr, std::streamsize count, void*);
    static void log_cerr(const char* ptr, std::streamsize count, void*);

private:
    static QString aboutMessage;

private:
    Scene *scene;
    GLWidget *glWidget;
    QMenu *fileMenu;
    QMenu *helpMenu;
    QAction *exitAct;
    QAction *aboutAct;
    StdRedirector<> *m_std_cout_logger;
    StdRedirector<> *m_std_cerr_logger;
};

}

#endif // VVR_WINDOW_H
