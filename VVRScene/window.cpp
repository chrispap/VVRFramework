#include "window.h"
#include "glwidget.h"
#include "scene.h"
#include "logger.h"

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

    // Redirect std::cout to our custom logging widget
    m_std_cout_logger = new StdRedirector<>(std::cout, &Window::log_cout, plain_text_log);
    m_std_cerr_logger = new StdRedirector<>(std::cerr, &Window::log_cerr, plain_text_log);

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

    if (scene->createMenus()) 
        createMenus();

    setWindowTitle(tr(scene->getName()));

    if (scene->fullScreen()){
        showFullScreen();
    }
    else {
        show();
    }

    glWidget->setFocus();
}

void vvr::Window::log_cout(const char* ptr, std::streamsize count, void* pte)
{
    QString str = QString::fromLocal8Bit(ptr, count);
    QPlainTextEdit* te = static_cast<QPlainTextEdit*>(pte);
    QScrollBar *vScrollBar = te->verticalScrollBar();
    bool keep_on_bottom = vScrollBar->value() == vScrollBar->maximum();

    //te->appendHtml("<font color=\"Red\">" + str + "</font>");

    te->moveCursor(QTextCursor::End);
    te->textCursor().insertHtml("<font color=\"White\">" + str + "</font>");
    te->moveCursor(QTextCursor::End);
    if (ptr[(int)count - 1] == '\n') te->appendPlainText("");

    if (keep_on_bottom) {
        vScrollBar->triggerAction(QScrollBar::SliderToMaximum);
    }
    printf("%.*s", (int) count, ptr);
    // vvr::logi(str.toStdString());
}

void vvr::Window::log_cerr(const char* ptr, std::streamsize count, void* pte)
{
    QString str = QString::fromLocal8Bit(ptr, count);
    QPlainTextEdit* te = static_cast<QPlainTextEdit*>(pte);
    QScrollBar *vScrollBar = te->verticalScrollBar();
    bool keep_on_bottom = vScrollBar->value() == vScrollBar->maximum();
    
    //te->appendHtml("<font color=\"Red\">" + str + "</font>");
    
    te->moveCursor(QTextCursor::End); 
    te->textCursor().insertHtml("<font color=\"Red\">" + str + "</font>");
    te->moveCursor(QTextCursor::End);
    if (ptr[(int)count - 1] == '\n') te->appendPlainText("");
    
    if (keep_on_bottom) {
        vScrollBar->triggerAction(QScrollBar::SliderToMaximum);
    }
    fprintf(stderr, "%.*s", (int)count, ptr);
    //vvr::loge(str.toStdString());
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
    QApplication app(argc, argv);
    QPixmap pixmap(":/Icons/vvrframework-splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();
    Window window(scene);
    window.show();
    splash.close();
    app.exec();
    return 0;
}
