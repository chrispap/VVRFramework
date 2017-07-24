#include "tavli.h"
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <set>
#include <vector>
#include <MathGeoLib.h>

int main(int argc, char* argv[])
{
    try {
        return vvr::mainLoop(argc, argv, new tavli::Scene());
    }
    catch (std::string exc) {
        std::cerr << exc << std::endl;
        return 1;
    }
}

/*-------------------------------------------------------------------------------------*/

using math::vec;

tavli::Scene::Scene()
{
    m_bg_col = vvr::Colour("3d2001");
    m_fullscreen = true;
    mCanvas.setDelOnClear(false);
}

void tavli::Scene::reset()
{
    vvr::Scene::reset();
}

void tavli::Scene::resize()
{
    static bool first_pass = true;
    if (first_pass) {
        mCanvas.add(mAxes = getGlobalAxes())->show();
        mCanvas.add(mBoard = new Board(1024,768))->show();
        reset();
    }
    first_pass = false;
}

void tavli::Scene::draw()
{
    enterPixelMode();
    mCanvas.draw();
    returnFromPixelMode();
}

void tavli::Scene::mousePressed(int x, int y, int modif)
{
    const bool shift_down = shiftDown(modif);
    return vvr::Scene::mousePressed(x, y, modif);
}

void tavli::Scene::mouseMoved(int x, int y, int modif)
{
    const bool shift_down = shiftDown(modif);
    return vvr::Scene::mouseMoved(x, y, modif);
}

void tavli::Scene::mouseReleased(int x, int y, int modif)
{
    const bool shift_down = shiftDown(modif);
    return vvr::Scene::mouseReleased(x, y, modif);
}

void tavli::Scene::keyEvent(unsigned char key, bool up, int modif)
{
    vvr::Scene::keyEvent(key, up, modif);
    key = tolower(key);

    switch (key)
    {
    case 'a': 
        mAxes->toggleVisibility();
        break;
    }
}

const char* tavli::Scene::getName() const
{ 
    return "Tavli Game";
}

/*-------------------------------------------------------------------------------------*/

tavli::Piece::Piece()
{
}

void tavli::Piece::draw() const
{
}

/*-------------------------------------------------------------------------------------*/

tavli::Region::Region()
{
}

void tavli::Region::draw() const
{
}

/*-------------------------------------------------------------------------------------*/

tavli::Board::Board(float w, float h)
    : w(w), h(h)
{
    canvas.setDelOnClear(false);
    canvas.add(new vvr::LineSeg2D(-w / 2, -h / 2, -w / 2, h / 2));
    canvas.add(new vvr::LineSeg2D(-w / 2, h / 2, w / 2, h / 2));
    canvas.add(new vvr::LineSeg2D(w / 2, h / 2, w / 2, -h / 2));
    canvas.add(new vvr::LineSeg2D(w / 2, -h / 2, -w / 2, -h / 2));
}

void tavli::Board::draw() const
{
    vvr::Shape::LineWidth = 4;
    canvas.draw();
}

/*-------------------------------------------------------------------------------------*/
