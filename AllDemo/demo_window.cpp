#include "demo_window.h"
#include "Apps/Boxes.cpp"
#include "Apps/Origami.cpp"
#include "Apps/DNAHelix.cpp"
#include "Apps/ContourEditor.cpp"
#include "Apps/Sketcher2D.cpp"
#include "Apps/LilacChaser.cpp"
#include "Apps/ArmJoint.cpp"
#include "Apps/Simple2D.cpp"
#include "Apps/Simple3D.cpp"
#include "GeoLab/Molding.cpp"
#include "GeoLab/Triangulation.cpp"
#include "GeoLab/Mesh3D.cpp"

/*--------------------------------------------------------------------------------------*/
DemoWindow::DemoWindow()
{
    ui.setupUi(this);
    glw = nullptr;

    scns.push_back(new OrigamiScene);
    scns.push_back(new BoxesScene);
    scns.push_back(new HelixScene);
    scns.push_back(new ContourEditorScene);
    scns.push_back(new Sketcher);
    scns.push_back(new LilacChaserScene);
    scns.push_back(new ArmJointScene);
    scns.push_back(new Simple2DScene);
    scns.push_back(new Simple3DScene);
    scns.push_back(new MoldingScene);
    scns.push_back(new TriangulationScene);
    scns.push_back(new Mesh3DScene);

    /* Add scenes to list */
    for (auto s : scns) {
        auto b = new QPushButton(s->getName());
        connect(b, &QPushButton::clicked, [s,this](){
            SetScene(s);
        });
        ui.verticalLayout_left->addWidget(b);
    }

    /* Set initial scene */
    SetScene(scns.at(0));
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
    } else glw->setScene(scn);
}

/*--------------------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    DemoWindow demowin;
    demowin.showMaximized();
    app.exec();
}
