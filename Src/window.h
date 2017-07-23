#ifndef VVR_WINDOW_H
#define VVR_WINDOW_H

#include "ui_window.h"
#include "stdout_redirector.h"
#include <vvr/glwidget.h>
#include <vvr/scene.h>

namespace vvr {

class vvrframework_API Window : public QMainWindow, private Ui::MainWindow
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
    void keyPressEvent(QKeyEvent* event) override;
    void createActions();
    void createMenus();

private: // Static callbacks for StdRedirector
    static void s_log_cout(const char* ptr, std::streamsize count, void*);
    static void s_log_cerr(const char* ptr, std::streamsize count, void*);

signals: // signals emmited from statuc callbacks
    void log_cout(const QString &str);
    void log_cerr(const QString &str);

private slots: // Slots that actually show the text
    void do_log_cout(const QString &str);
    void do_log_cerr(const QString &str);

private:
    static QString aboutMessage;

private:
    QMenu *fileMenu;
    QMenu *helpMenu;
    QAction *exitAct;
    QAction *aboutAct;
    StdRedirector<> *m_std_cout_logger;
    StdRedirector<> *m_std_cerr_logger;

protected:
    Scene *scene;
    GLWidget *glWidget;
};

}

#endif
