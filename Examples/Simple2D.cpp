#include "Simple2D.h"
#include <vvr/utils.h>
#include <vvr/canvas.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Simple2DScene::Simple2DScene()
{
    m_bg_col = vvr::Colour::grey;
    m_rad = 20;

    // Add 5 circles to our canvas.
    m_canvas.add(new vvr::Circle2D( -40, -20, 40, vvr::Colour::red));
    m_canvas.add(new vvr::Circle2D( -20,  20, 40, vvr::Colour::green));
    m_canvas.add(new vvr::Circle2D(   0, -20, 40, vvr::Colour::blue));
    m_canvas.add(new vvr::Circle2D(  20,  20, 40, vvr::Colour::black));
    m_canvas.add(new vvr::Circle2D(  40, -20, 40, vvr::Colour::yellow));
    m_canvas.newFrame(true);
}

void Simple2DScene::draw()
{
    enterPixelMode();
    m_canvas.draw();
    returnFromPixelMode();
}

void Simple2DScene::mousePressed(int x, int y, int modif)
{
    m_canvas.newFrame();
    m_canvas.add(new vvr::Circle2D(x, y, m_rad, vvr::Colour::black));
}

void Simple2DScene::mouseMoved(int x, int y, int modif)
{
    m_canvas.add(new vvr::Circle2D(x, y, m_rad, vvr::Colour::red));
}

void Simple2DScene::mouseWheel(int dir, int modif)
{
    if (dir>0) {
        m_rad += 1;
    }
    else {
        m_rad -= 1;
        if (m_rad<=0) m_rad = 1;
    }

}

void Simple2DScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir==vvr::LEFT) {
        m_canvas.prev();
    }
    else if (dir==vvr::RIGHT) {
        m_canvas.next();
    }

}

void Simple2DScene::reset()
{
    vvr::Scene::reset();
    m_canvas.resize(1);
    m_canvas.newFrame();
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new Simple2DScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
        return 1;
    }
}
