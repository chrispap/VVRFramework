#include "window.h"
#include <vvr/glwidget.h>
#include <vvr/scene.h>
#include <vvr/command.h>
#include <QtWidgets>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDir>
#include <iostream>

using std::cerr;
using std::endl;

QString vvr::Window::aboutMessage = QString("Chris Papapavlou | chrispapapaulou@gmail.com ") + QString(QChar(0xA9));

vvr::Window::Window(vvr::Scene *scene) : m_scene(scene)
{
    setupUi(this);
    setWindowTitle(tr(scene->getName()));

    /* Set commands availabe to scene */
    scene->cursorShow.add(new vvr::SimpleCmd<Window>(this, &Window::cursor_show));
    scene->cursorHide.add(new vvr::SimpleCmd<Window>(this, &Window::cursor_hide));
    scene->cursorHand.add(new vvr::SimpleCmd<Window>(this, &Window::cursor_hand));
    scene->cursorGrab.add(new vvr::SimpleCmd<Window>(this, &Window::cursor_grab));

    /* Redirect std::cout to our custom logging widget */
    m_std_cout_logger = new StdRedirector<>(std::cout, &Window::s_log_cout, this);
    m_std_cerr_logger = new StdRedirector<>(std::cerr, &Window::s_log_cerr, this);

    /* Init glwidget */
    m_glwidget = new vvr::GlWidget(scene);
    installEventFilter(m_glwidget);
    scrollArea->setWidget(m_glwidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(horizontalSlider_0, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
    connect(horizontalSlider_1, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
    connect(horizontalSlider_2, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
    connect(horizontalSlider_3, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
    connect(horizontalSlider_4, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
    connect(horizontalSlider_5, SIGNAL(valueChanged(int)), this, SLOT(sliderMoved(int)));
    createActions();

    if (scene->getCreateMenus()) {
        createMenus();
    }
    if (scene->getHideLog()) {
        plain_text_log->hide();
    }
    if (scene->getHideSliders()) {
        QLayout * layout = slider_groupbox->layout();
        QLayoutItem * item;
        QLayout * sublayout;
        QWidget * widget;
        while ((item = layout->takeAt(0))) {
            if ((sublayout = item->layout()) != 0) {/* do the same for sublayout*/}
            else if ((widget = item->widget()) != 0) { widget->hide(); delete widget; }
            else { delete item; }
        }
        delete layout;
        delete slider_groupbox;
    }
    if (scene->getFullScreen()) {
        QTimer::singleShot(150, this, SLOT(showFullScreen()));
    }
    else {
        showNormal();
    }
}

void vvr::Window::createActions()
{
    //! Action Exit
    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
    //! Action About
    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void vvr::Window::createMenus()
{
    //! Menu: File
    fileMenu = QMainWindow::menuBar()->addMenu(tr("&File"));
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    //! Menu: Help
    helpMenu = QMainWindow::menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void vvr::Window::about()
{
    QMessageBox::about(this, m_scene->getName(), aboutMessage);
}

void vvr::Window::sliderMoved(int val)
{
    const int id = std::stoi(vvr::split(sender()->objectName().toStdString(), '_').back());
    m_scene->sliderChanged(id, val / 100.0f);
    m_glwidget->update();
}

void vvr::Window::focusToGlWidget()
{
    m_glwidget->setFocus();
}

void vvr::Window::s_log_cout(const char* ptr, std::streamsize count, void* pte)
{
    printf("%.*s", (int)count, ptr);
    vvr::Window *window = static_cast<vvr::Window*>(pte);
    const QString str = QString::fromLocal8Bit(ptr, count);
    window->log_cout(str);
}

void vvr::Window::s_log_cerr(const char* ptr, std::streamsize count, void* pte)
{
    fprintf(stderr, "%.*s", (int)count, ptr);
    vvr::Window *window = static_cast<vvr::Window*>(pte);
    const QString str = QString::fromLocal8Bit(ptr, count);
    window->log_cerr(str);
}

void vvr::Window::log_cout(const QString &str)
{
    QScrollBar *vScrollBar = plain_text_log->verticalScrollBar();
    const bool keep_on_bottom = vScrollBar->value() == vScrollBar->maximum();

    //! Print text
    plain_text_log->moveCursor(QTextCursor::End);
    QString html = str.toHtmlEscaped();
    html.replace(QChar(' '), QString("&nbsp;"), Qt::CaseInsensitive);
    plain_text_log->textCursor().insertHtml("<font color=\"White\">" + html + "</font>");
    plain_text_log->moveCursor(QTextCursor::End);

    //! New line
    if (str.at(str.length() - 1) == '\n') {
        plain_text_log->appendPlainText("");
    }

    if (keep_on_bottom) {
        vScrollBar->triggerAction(QScrollBar::SliderToMaximum);
    }
}

void vvr::Window::log_cerr(const QString &str)
{
    QScrollBar *vScrollBar = plain_text_log->verticalScrollBar();
    const bool keep_on_bottom = vScrollBar->value() == vScrollBar->maximum();

    //! Print text
    QString html = str.toHtmlEscaped();
    html.replace(QChar(' '), QString("&nbsp;"), Qt::CaseInsensitive);
    plain_text_log->moveCursor(QTextCursor::End);
    plain_text_log->textCursor().insertHtml("<font color=\"Red\">" + html + "</font>");
    plain_text_log->moveCursor(QTextCursor::End);

    //! New line
    if (str.at(str.length() - 1) == '\n') {
        plain_text_log->appendPlainText("");
    }

    if (keep_on_bottom) {
        vScrollBar->triggerAction(QScrollBar::SliderToMaximum);
    }
}

void vvr::Window::cursor_show()
{
    QApplication::changeOverrideCursor(Qt::CrossCursor);
}

void vvr::Window::cursor_hide()
{
    QApplication::changeOverrideCursor(Qt::BlankCursor);
}

void vvr::Window::cursor_hand()
{
    QApplication::changeOverrideCursor(Qt::OpenHandCursor);
}

void vvr::Window::cursor_grab()
{
    QApplication::changeOverrideCursor(Qt::ClosedHandCursor);
}

int vvr::main_with_scene(int argc, char* argv[], vvr::Scene *scene)
{
    QApplication app(argc, argv);
    QPixmap pixmap(":/Icons/vvrframework-splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();
    Window win(scene);
    splash.close();
    win.show();//Maximized();
    win.focusToGlWidget();
    app.setOverrideCursor(Qt::CrossCursor);
    app.exec();
    return 0;
}
