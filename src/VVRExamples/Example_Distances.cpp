#include "Example_Distances.h"
#include "utils.h"
#include "canvas.h"
#include <iostream>
#include <fstream>
#include <string>

using std::vector;
using std::string;
using std::cout;
using std::endl;

#define APP_TITLE "Simple 2D Drawing"

const char* DistancesScene::getName() const
{
    return APP_TITLE;
}

DistancesScene::DistancesScene()
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

void DistancesScene::draw()
{
    enterPixelMode();
    m_canvas.draw();
    returnFromPixelMode();
}

void DistancesScene::mousePressed(int x, int y, int modif)
{
    m_canvas.newFrame();
    m_canvas.add(new vvr::Circle2D(x, y, m_rad, vvr::Colour::black));
}

void DistancesScene::mouseMoved(int x, int y, int modif)
{
    m_canvas.add(new vvr::Circle2D(x, y, m_rad, vvr::Colour::red));
}

void DistancesScene::mouseWheel(int dir, int modif)
{
    if (dir>0) {
        m_rad += 1;
    }
    else {
        m_rad -= 1;
        if (m_rad<=0) m_rad = 1;
    }

}

void DistancesScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir==vvr::LEFT) {
        m_canvas.prev();
    }
    else if (dir==vvr::RIGHT) {
        m_canvas.next();
    }

}

void DistancesScene::reset()
{
    vvr::Scene::reset();
    m_canvas.resize(1);
    m_canvas.newFrame();
}

int main(int argc, char* argv[])
{
    return vvr::mainLoop(argc, argv, new DistancesScene);
}
