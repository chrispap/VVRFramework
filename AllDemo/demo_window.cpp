#include "demo_window.h"
#include "../Apps/Boxes.cpp"
#include "../Apps/Origami.cpp"
#include "../Apps/DNAHelix.cpp"
#include "../Apps/ContourEditor.cpp"
#include "../Apps/Sketcher2D.cpp"
#include "../Apps/LilacChaser.cpp"
#include "../Apps/ArmJoint.cpp"
#include "../Apps/Tutorial.cpp"
#include "../Apps/Simple2D.cpp"
#include "../Apps/Simple3D.cpp"

DemoWindow::DemoWindow()
{
    glw = nullptr;
    ui.setupUi(this);

    scns.push_back(new OrigamiScene);
    scns.push_back(new BoxesScene);
    scns.push_back(new HelixScene);
    scns.push_back(new ContourEditorScene);
    scns.push_back(new Sketcher);
    scns.push_back(new LilacChaserScene);
    scns.push_back(new ArmJointScene);
    scns.push_back(new Simple2DScene);
    scns.push_back(new Simple3DScene);
    scns.push_back(new TutorialScene);

    for (auto s:scns) {
        auto b = new QPushButton(s->getName());
        connect(b, &QPushButton::clicked, [s,this](){SetScene(s);});
        ui.verticalLayout_left->addWidget(b);
    }
}

void DemoWindow::SetScene(vvr::Scene *scene)
{
    scn = scene;
    if (!glw) {
        glw = new vvr::GlWidget(scn);
        ui.scrollArea->setWidget(glw);
        ui.scrollArea->setWidgetResizable(true);
        ui.scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ui.scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        connect(this, SIGNAL(keyPressed(QKeyEvent*)), glw, SLOT(onKeyPressed(QKeyEvent*)));
    } else glw->setScene(scn);
}

void DemoWindow::keyPressEvent(QKeyEvent* event)
{
    std::string str = event->text().toStdString();
    if (str.length() > 0) emit keyPressed(event);
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    DemoWindow demowin;
    demowin.showMaximized();
    app.exec();
}
