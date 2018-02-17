#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <GeoLib.h>
#include <MathGeoLib.h>
#include <iostream>
#include <fstream>
#include <string>

struct TutorialScene : public vvr::Scene
{
    TutorialScene();
private:
    const char* getName() const override { return "VVRFramework Tutorial"; }
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;
    vvr::Canvas m_canvas;
};

using namespace std;
using namespace vvr;
using namespace math;

static C2DPoint p1, p2;

TutorialScene::TutorialScene()
{
    m_bg_col = vvr::grey;
    m_hide_log = false;
}

void TutorialScene::draw()
{
    cout << "Drawing" << endl;
    enterPixelMode();
    m_canvas.draw();
    exitPixelMode();
}

void TutorialScene::mousePressed(int x, int y, int modif)
{
    cout << "Mouse pressed - [" << x << ", " << y << "]" << endl;

    C2DPoint p3(x,y);
    C2DTriangle triangle(p1, p2, p3);
    m_canvas.add(triangle);
    p1 = p2;
    p2 = p3;
}

void TutorialScene::mouseMoved(int x, int y, int modif)
{
    cout << "Mouse moved   - [" << x << ", " << y << "]" << endl;

    C2DPoint p2(x,y);
    m_canvas.add(p1, p2);
    m_canvas.add(p1, vvr::darkRed);
    p1 = p2;
}

void TutorialScene::mouseWheel(int dir, int modif)
{
    if (dir>0)
    {
        cout << "Mouse wheel   - [Up]" << endl;
    }
    else
    {
        cout << "Mouse wheel   - [Down]" << endl;
    }
}

void TutorialScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir==vvr::LEFT)
    {
        cout << "Arrow event   - [Left]" << endl;
    }
    else if (dir==vvr::RIGHT)
    {
        cout << "Arrow event   - [Right]" << endl;
    }
    else if (dir==vvr::UP)
    {
        cout << "Arrow event   - [Up]" << endl;
    }
    else if (dir==vvr::DOWN)
    {
        cout << "Arrow event   - [Down]" << endl;
    }
}

void TutorialScene::reset()
{
    cout << "Reset" << endl;
    vvr::Scene::reset();
    m_canvas.clear();
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(TutorialScene)
#endif
