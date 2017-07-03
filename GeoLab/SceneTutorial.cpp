#include "SceneTutorial.h"
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <GeoLib.h>
#include <MathGeoLib.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace vvr;
using namespace math;

int x;
int y;

TutorialScene::TutorialScene()
{
    m_bg_col = vvr::Colour::grey;
    m_hide_log = false;
}

void TutorialScene::draw()
{
    enterPixelMode();
    canvas.draw();
    returnFromPixelMode();
}

void TutorialScene::mousePressed(int x, int y, int modif)
{
    cout << "Mouse pressed - [" << x << ", " << y << "]" << endl;

    C2DPoint p(x, y);
    canvas.add(p, vvr::Colour::green);

}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new TutorialScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
        return 1;
    }
}
