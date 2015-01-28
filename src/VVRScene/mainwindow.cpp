#include "mainwindow.h"
#include "glwidget.h"
#include "scene.h"

#include <QtOpenGL>
#include <QtWidgets>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDir>
#include <iostream>

QString MainWindow::aboutMessage = QString("VVR LAB 2014") + QString(QChar(0xA9));

MainWindow::MainWindow(vvr::Scene *scene)
{
    setupUi(this);
    this->scene = scene;

    // Init glwidget
    glWidget = new GLWidget(scene);
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
}

MainWindow::~MainWindow()
{

}

void MainWindow::createActions()
{
    // exit action
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    // about action
    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
{
    // file menu
    fileMenu = QMainWindow::menuBar()->addMenu(tr("&File"));
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    // help menu
    helpMenu = QMainWindow::menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::about()
{
    QMessageBox::about(this, scene->getName(), aboutMessage);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    std::string str = event->text().toStdString();
    if (str.length()>0) emit keyPressed(event);
}

/*  Entry point of VVR Scene Framework */
int vvr::main(int argc, char* argv[], vvr::Scene *scene)
{
    try {
        QApplication app(argc, argv);
        MainWindow window(scene);
        window.show();
        app.exec();
    }
    catch (string exc) {
        cerr << exc << endl;
        return 1;
    }

    return 0;
}
