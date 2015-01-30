#include "simple2dscene.h"
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

Simple2DScene::Simple2DScene() : m_settings(getExePath() + CONFIGFILEPATH)
{
    bgCol = vvr::ColRGB::grey;
    m_rad = 20;
    load();
}

void Simple2DScene::load()
{
    // Make 5 circles and add them to the canvas.
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
    // Transform mouse click coords to pixel scene coords.
    x -= screen_width/2;
    y -= screen_height/2;
}

void Simple2DScene::mousePressed(int x, int y, int modif)
{
    mouse2pix(x,y);
    m_canvas.add(new vvr::Circle2D(x, y, m_rad, vvr::ColRGB::black));
}

void Simple2DScene::mouseMoved(int x, int y, int modif)
{
    mouse2pix(x,y);
    m_canvas.add(new vvr::Circle2D(x, y, m_rad, vvr::ColRGB::blue));
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

void Simple2DScene::reset()
{
    vvr::Scene::reset();
    m_canvas.resize(1);
    m_canvas.newFrame(true);
}
