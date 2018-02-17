#include <ui_demo_window.h>
#include <vvr/glwidget.h>
#include <QApplication>
#include <QDialog>

class DemoWindow : public QMainWindow
{
    Q_OBJECT
public:
    DemoWindow();
    void SetScene(vvr::Scene*);
signals:
    void keyPressed(QKeyEvent*);
private slots:
    void keyPressEvent(QKeyEvent*) override;
private:
    Ui::DemoWindow ui;
    vvr::GlWidget *glw;
    vvr::Scene *scn;
    std::vector<vvr::Scene*> scns;
};
