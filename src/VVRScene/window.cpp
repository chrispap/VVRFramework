#include "window.h"
#include "glwidget.h"
#include "scene.h"

#include <QtOpenGL>
#include <QtWidgets>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDir>
#include <iostream>

using std::cerr;
using std::endl;

QString vvr::Window::aboutMessage = QString("VVR LAB 2014") + QString(QChar(0xA9));

vvr::Window::Window(vvr::Scene *scene)
{
    setupUi(this);
    this->scene = scene;

    // Init glwidget
    glWidget = new vvr::GLWidget(scene);
    scrollArea->setWidget(glWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Make connections
    connect(this, SIGNAL(keyPressed(QKeyEvent*)), glWidget, SLOT(onKeyPressed(QKeyEvent*)));

    // Create actions, menus, etc.
    createActions();
    createMenus();
    setWindowTitle(tr(scene->getName()));
    glWidget->setFocus();
    //showMaximized();
}

void vvr::Window::createActions()
{
    // exit action
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    // about action
    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void vvr::Window::createMenus()
{
    // file menu
    fileMenu = QMainWindow::menuBar()->addMenu(tr("&File"));
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    // help menu
    helpMenu = QMainWindow::menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void vvr::Window::about()
{
    QMessageBox::about(this, scene->getName(), aboutMessage);
}

void vvr::Window::keyPressEvent(QKeyEvent* event)
{
    std::string str = event->text().toStdString();
    if (str.length()>0) emit keyPressed(event);
}

int vvr::mainLoop(int argc, char* argv[], vvr::Scene *scene)
{
    try {
        QApplication app(argc, argv);
        Window window(scene);
        window.show();
        app.exec();
    }
    catch (string exc) {
        cerr << exc << endl;
        return 1;
    }

    return 0;
}
