#include <ui_demo_window.h>
#include <vvr/glwidget.h>

class DemoWindow : public QMainWindow
{
    Q_OBJECT
public:
    DemoWindow();
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
