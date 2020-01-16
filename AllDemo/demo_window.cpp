#include <ui_demo_window.h>
#include <vvr/glwidget.h>
#include <QPushButton>
#include <string>
//! Scene cpp files
#include "Apps/Boxes.cpp"
#include "Apps/Origami.cpp"
#include "Apps/DNAHelix.cpp"
#include "Apps/ContourEditor.cpp"
#include "Apps/Sketcher2D.cpp"
#include "Apps/LilacChaser.cpp"
#include "Apps/ArmJoint.cpp"
#include "Apps/Simple2D.cpp"
#include "Apps/Simple3D.cpp"
#include "Apps/Tutorial.cpp"
#include "Apps/KDTrees.cpp"
#include "Apps/Fourier.cpp"
#include "Apps/Stadium.cpp"
#include "GeoLab/Molding.cpp"
#include "GeoLab/Triangulation.cpp"
#include "GeoLab/Mesh3D.cpp"
#include "GeoLab/Triangle.cpp"
#include "GeoLab/ConvexHull.cpp"
#include "Games/tavli.cpp"

/*--------------------------------------------------------------------------------------*/
template <typename T>
static void gather(T &scenes)
{
    scenes.push_back(new OrigamiScene);
    scenes.push_back(new BoxesScene);
    scenes.push_back(new HelixScene);
    scenes.push_back(new ContourEditorScene);
    scenes.push_back(new Sketcher);
    scenes.push_back(new LilacChaserScene);
    scenes.push_back(new ArmJointScene);
    scenes.push_back(new Simple2DScene);
    scenes.push_back(new Simple3DScene);
    scenes.push_back(new MoldingScene);
    scenes.push_back(new DelaunayScene);
    scenes.push_back(new DelaunayAutoScene);
    scenes.push_back(new Mesh3DScene);
    scenes.push_back(new KDTreeScene);
    scenes.push_back(new TutorialScene);
    scenes.push_back(new TriangleScene);
    scenes.push_back(new ConvexHullScene);
    scenes.push_back(new FourierScene);
    scenes.push_back(new StadiumScene);
    scenes.push_back(new TavliScene(tavli::GetDefaultColours()));
}

/*--------------------------------------------------------------------------------------*/
class DemoWindow : public QMainWindow
{
    Q_OBJECT
public:
    DemoWindow();
    ~DemoWindow();
public slots:
    void SetScene(vvr::Scene*);
signals:
    void keyPressed(QKeyEvent*);
private:
    Ui::DemoWindow ui;
    vvr::GlWidget *glw;
    vvr::Scene *scn;
    std::vector<vvr::Scene*> scenes;
};

DemoWindow::DemoWindow()
{
    ui.setupUi(this);
    glw = nullptr;

    /* Add scenes to list */
    gather(scenes);
    int i = 1;
    for (auto s : scenes) {
        auto label = QString::fromStdString(std::to_string(i++) + ". " + s->getName());
        auto btn = new QPushButton(label);
        btn->setStyleSheet("Text-align:left");
        connect(btn, &QPushButton::clicked, [s,this]() { SetScene(s); });
        ui.layout_scenes->addWidget(btn);
    }

    /* Set initial scene */
    SetScene(scenes[0]);
    installEventFilter(glw);
    glw->setFocus();
}

DemoWindow::~DemoWindow()
{
    qDeleteAll(scenes);
}

void DemoWindow::SetScene(vvr::Scene *scene)
{
    scn = scene;
    if (!glw) {
        glw = new vvr::GlWidget(scn);
        ui.scrollArea->setWidget(glw);
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

#include "demo_window.moc"
