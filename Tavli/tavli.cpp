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
    try 
    {
        return vvr::mainLoop(argc, argv, new tavli::Scene());
    }
    catch (std::string exc) 
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}

/*--- [Scene] ---------------------------------------------------------------------------*/

using math::vec;

tavli::Scene::Scene()
{
    m_bg_col = vvr::Colour("3d2001");
    m_hide_log = false;
    mBoard = new Board();
}

void tavli::Scene::reset()
{
    vvr::Scene::reset();
}

void tavli::Scene::resize()
{
    static bool first_pass = true;
    if (first_pass) {
        first_pass = false;
        mAxes = getGlobalAxes();
        mAxes->hide();
    }
    mBoard->resize(0.88*getViewportWidth(), 0.88*getViewportHeight());
}

void tavli::Scene::draw()
{
    enterPixelMode();
    mBoard->draw();
    mAxes->drawif();
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

/*--- [Tavli entities] ----------------------------------------------------------------*/

void tavli::Piece::draw() const
{
    circle.draw();
    vvr::Circle2D c(circle);
    c.r *= 0.70;
    c.colour = vvr::darkRed;
    c.draw();
}

void tavli::Region::draw() const
{
}

tavli::Board::Board()
{
    /* Bounds */
    bounds.resize(4);
    for (size_t i = 0; i < 4; ++i) {
        canvas.add(bounds[i] = new vvr::LineSeg2D());
    }

    /* The wood in the middle */
    wood.resize(2);
    for (size_t i = 0; i < 2; ++i) {
        canvas.add(wood[i] = new vvr::Triangle2D());
        wood[i]->filled = true;
    }

    /* Pieces */
    pieces.resize(15);
    for (size_t i = 0; i < pieces.size(); ++i) {
        canvas.add(pieces[i] = new Piece());
        pieces[i]->circle.filled = true;
        pieces[i]->circle.colour = vvr::Colour(0x95, 0, 0);
    }
}

void tavli::Board::resize(float width, float height)
{
    w = width;
    h = height;

    const float pdiam = w / 13;
    const float prad = pdiam / 2;

    /* Bounds */
    do {
        size_t i;
        i = 0;
        bounds[i++]->set(-w / 2, -h / 2, -w / 2, +h / 2);
        bounds[i++]->set(-w / 2, +h / 2, +w / 2, +h / 2);
        bounds[i++]->set(+w / 2, +h / 2, +w / 2, -h / 2);
        bounds[i++]->set(+w / 2, -h / 2, -w / 2, -h / 2);
        i = 0;
        wood[i++]->set(-prad, -height / 2, -prad, height / 2, prad, +height / 2);
        wood[i++]->set(-prad, -height / 2, +prad, height / 2, prad, -height / 2);
    } while (0);

    /* Pieces */
    for (size_t i = 0; i < pieces.size(); ++i) 
    {
        float x, y;
        size_t regcol = i % 6;
        size_t regrow = i / 6;
        x = pdiam + pdiam * regcol;
        y = -h / 2 + prad + regrow * pdiam;
        pieces[i]->circle.set(x, y, pdiam * 0.48);
    }
}

void tavli::Board::draw() const
{
    vvr::Shape::LineWidth = 8;
    canvas.draw();
}

/*-------------------------------------------------------------------------------------*/
