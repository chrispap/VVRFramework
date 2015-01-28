#include "simple2dscene.h"
#include "utils.h"
#include "canvas.h"

#include <Mathgeolib/MathGeoLib.h>
#include <iostream>
#include <fstream>
#include <string>

#define CONFIGFILEPATH "../../config/settings.txt"

using std::vector;
using std::string;
using std::cout;
using std::endl;

const char* Simple2DScene::getName() const
{
    return "Simple 2D Drawing";
}

Simple2DScene::Simple2DScene() : m_settings(getExePath() + CONFIGFILEPATH)
{
    // Read params from configuration file
    camera_dist = 100;
    scene_width = 100;
    scene_height = 100;
    bgCol = vvr::ColRGB(m_settings.getStr("color_bg"));
    perspective_proj = 0;
    globRotDef = vvr::Vec3d(0,0,0);
    globRot = globRotDef;

    load();
}

void Simple2DScene::load()
{
    // Make 5 circles and add them to the canvas.

    vvr::Circle2D* c[5];

    int i=0;
    c[i++] = new vvr::Circle2D(-6,  2, 4, vvr::ColRGB::red);
    c[i++] = new vvr::Circle2D(-3, -2, 4, vvr::ColRGB::green);
    c[i++] = new vvr::Circle2D( 0,  2, 4, vvr::ColRGB::black);
    c[i++] = new vvr::Circle2D( 3, -2, 4, vvr::ColRGB::yellow);
    c[i++] = new vvr::Circle2D( 6,  2, 4, vvr::ColRGB::blue);

    for (i=0; i<5; i++)
        m_canvas.add(c[i]);

}

void Simple2DScene::draw()
{
    drawAxes();
    m_canvas.draw();
}

bool Simple2DScene::idle()
{
    return 0;
}
