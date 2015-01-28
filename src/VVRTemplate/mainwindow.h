#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtOpenGL>
#include <QMainWindow>
#include "scene.h"
#include "ui_mainwindow.h"

class GLWidget;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(vvr::Scene *scene);
    ~MainWindow();

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

#endif // MAINWINDOW_H
