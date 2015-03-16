#include "Ex_01_Simple2DScene.h"
#include "utils.h"
#include "canvas.h"

#include <MathGeoLib/MathGeoLib.h>
#include <iostream>
#include <fstream>
#include <string>

using std::vector;
using std::string;
using std::cout;
using std::endl;

#define APP_TITLE "Simple 2D Drawing"
#define CONFIGFILEPATH "../../config/settings.txt"

const char* Simple2DScene::getName() const
{
    return APP_TITLE;
}

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

void Simple2DScene::mouse2pix(int &x, int &y)
{
    // Transform mouse click coords to pixel scene coords
    // because we have moved the (0,0) at the viewport center.
    x -= m_screen_width/2;
    y -= m_screen_height/2;
}

void Simple2DScene::mousePressed(int x, int y, int modif)
{
    mouse2pix(x,y);
    m_canvas.newFrame();
    m_canvas.add(new vvr::Circle2D(x, y, m_rad, vvr::Colour::black));
}

void Simple2DScene::mouseMoved(int x, int y, int modif)
{
    mouse2pix(x,y);
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
