#ifndef VVR_WINDOW_H
#define VVR_WINDOW_H

#include "ui_window.h"
#include "stdout_redirector.h"
#include <vvr/glwidget.h>
#include <vvr/scene.h>

namespace vvr {

class VVRFramework_API Window : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    Window(vvr::Scene *scene);
    void focusToGlWidget();
    void cursor_show();
    void cursor_hide();
    void cursor_hand();
    void cursor_grab();

private slots:
    void about();
    void sliderMoved(int val);
    void createActions();
    void createMenus();
    void log_cout(const QString &str);
    void log_cerr(const QString &str);

private:
    QMenu *fileMenu;
    QMenu *helpMenu;
    QAction *exitAct;
    QAction *aboutAct;
    StdRedirector<> *m_std_cout_logger;
    StdRedirector<> *m_std_cerr_logger;
    GlWidget *m_glwidget;
    Scene *m_scene;

private:
    /* Static callbacks for StdRedirector */
    static void s_log_cout(const char* ptr, std::streamsize count, void*);
    static void s_log_cerr(const char* ptr, std::streamsize count, void*);
    static QString aboutMessage;
};

}

#endif
