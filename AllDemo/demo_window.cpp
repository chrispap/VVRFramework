#include <ui_demo_window.h>
#include <QPushButton>
#include <vvr/glwidget.h>
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
#include "GeoLab/Molding.cpp"
#include "GeoLab/Triangulation.cpp"
#include "GeoLab/Mesh3D.cpp"
#include "Games/tavli.cpp"

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
    std::vector<vvr::Scene*> scns;
};

static void gather_available_scenes(std::vector<vvr::Scene*> &scns)
{
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
    scns.push_back(new KDTreeScene);
    scns.push_back(new TutorialScene);
    scns.push_back(new TavliScene(tavli::GetDefaultColours()));
}

/*--------------------------------------------------------------------------------------*/
DemoWindow::DemoWindow()
{
    ui.setupUi(this);
    glw = nullptr;

    gather_available_scenes(scns);

    /* Add scenes to list */
    for (auto s : scns) {
        auto btn = new QPushButton(s->getName());
        ui.layout_scenes->addWidget(btn);
        connect(btn, &QPushButton::clicked, [s,this](){SetScene(s);});
    }

    /* Set initial scene */
    SetScene(scns[0]);
    installEventFilter(glw);
    glw->setFocus();
}

DemoWindow::~DemoWindow()
{
    qDeleteAll(scns);
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
    demowin.show();//Maximized();
    app.exec();
}

#include "demo_window.moc"
