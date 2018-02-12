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

signals:
    void keyPressed(QKeyEvent *event);
    void log_cout(const QString &str);
    void log_cerr(const QString &str);

private slots:
    void about();
    void sliderMoved(int val);
    void keyPressEvent(QKeyEvent* event) override;
    void createActions();
    void createMenus();
    void do_log_cout(const QString &str);
    void do_log_cerr(const QString &str);

private:
    QMenu *fileMenu;
    QMenu *helpMenu;
    QAction *exitAct;
    QAction *aboutAct;
    StdRedirector<> *m_std_cout_logger;
    StdRedirector<> *m_std_cerr_logger;
    GlWidget *glWidget;
    Scene *scene;

    /* Static callbacks for StdRedirector */
    static void s_log_cout(const char* ptr, std::streamsize count, void*);
    static void s_log_cerr(const char* ptr, std::streamsize count, void*);
    static QString aboutMessage;
};

}

#endif
