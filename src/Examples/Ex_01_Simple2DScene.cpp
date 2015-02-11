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
    bgCol = vvr::ColRGB::grey;
    m_rad = 20;

    // Add 5 circles to our canvas.
    m_canvas.add(new vvr::Circle2D( -40, -20, 40, vvr::ColRGB::red));
    m_canvas.add(new vvr::Circle2D( -20,  20, 40, vvr::ColRGB::green));
    m_canvas.add(new vvr::Circle2D(   0, -20, 40, vvr::ColRGB::blue));
    m_canvas.add(new vvr::Circle2D(  20,  20, 40, vvr::ColRGB::black));
    m_canvas.add(new vvr::Circle2D(  40, -20, 40, vvr::ColRGB::yellow));
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
    x -= screen_width/2;
    y -= screen_height/2;
}

void Simple2DScene::mousePressed(int x, int y, int modif)
{
    mouse2pix(x,y);
    m_canvas.newFrame();
    m_canvas.add(new vvr::Circle2D(x, y, m_rad, vvr::ColRGB::black));
}

void Simple2DScene::mouseMoved(int x, int y, int modif)
{
    mouse2pix(x,y);
    m_canvas.add(new vvr::Circle2D(x, y, m_rad, vvr::ColRGB::red));
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
    if (dir==vvr::ArrowDir::LEFT) {
        m_canvas.prev();
    }
    else if (dir==vvr::ArrowDir::RIGHT) {
        m_canvas.next();
    }

}

void Simple2DScene::reset()
{
    vvr::Scene::reset();
    m_canvas.resize(1);
    m_canvas.newFrame();
}
