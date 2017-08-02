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

using math::vec;

template <typename Dragger>
struct Picker2D
{
    void mousePressed(int x, int y, int modif)
    {
        vvr::Drawable *drw = nullptr;
        dragger.grab(drw, x, y);
    }

    void mouseMoved(int x, int y, int modif)
    {
        dragger.drag(drw, x, y);
    }

    void mouseReleased(int x, int y, int modif)
    {
        dragger.release(drw, x, y);
    }

    Picker2D(Dragger &dragger) 
        : dragger(dragger)
        , drw(nullptr)
    {

    }

private:
    Dragger &dragger;
    vvr::Drawable* drw;
    vec pickpos;
    vec currpos;
};

tavli::Scene::Scene()
{
    m_bg_col = vvr::Colour("3d2001");
    mBoard = new Board();
}

void tavli::Scene::reset()
{
    vvr::Scene::reset();
}

void tavli::Scene::resize()
{
    if (m_first_resize) 
    {
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
    exitPixelMode();
}

void tavli::Scene::mousePressed(int x, int y, int modif)
{
    vvr::Scene::mousePressed(x, y, modif);
}

void tavli::Scene::mouseMoved(int x, int y, int modif)
{
    vvr::Scene::mouseMoved(x, y, modif);
}

void tavli::Scene::mouseReleased(int x, int y, int modif)
{
    vvr::Scene::mouseReleased(x, y, modif);
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
    cir.draw();
    vvr::Circle2D c(cir);
    c.SetRadius(c.GetRadius() * 0.70);
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
    for (size_t i = 0; i < 4; ++i) 
    {
        canvas.add(bounds[i] = new vvr::LineSeg2D());
    }

    /* The wood in the middle */
    wood.resize(2);
    for (size_t i = 0; i < 2; ++i) 
    {
        canvas.add(wood[i] = new vvr::Triangle2D());
        wood[i]->filled = true;
    }

    /* Pieces */
    pieces.resize(15);
    for (size_t i = 0; i < pieces.size(); ++i) 
    {
        canvas.add(pieces[i] = new Piece());
        pieces[i]->cir.filled = true;
        pieces[i]->cir.colour = vvr::Colour(0x95, 0, 0);
    }
}

void tavli::Board::resize(float width, float height)
{
    w = width;
    h = height;

    const float pdiam = w / 13;
    const float prad = pdiam / 2;

    /* Bounds */
    size_t i;
    i = 0;
    bounds[i++]->set(-w / 2, -h / 2, -w / 2, +h / 2);
    bounds[i++]->set(-w / 2, +h / 2, +w / 2, +h / 2);
    bounds[i++]->set(+w / 2, +h / 2, +w / 2, -h / 2);
    bounds[i++]->set(+w / 2, -h / 2, -w / 2, -h / 2);
    i = 0;
    wood[i++]->set(-prad, -height / 2, -prad, height / 2, prad, +height / 2);
    wood[i++]->set(-prad, -height / 2, +prad, height / 2, prad, -height / 2);

    /* Pieces */
    for (size_t i = 0; i < pieces.size(); ++i) 
    {
        float x, y;
        size_t regcol = i % 6;
        size_t regrow = i / 6;
        x = pdiam + pdiam * regcol;
        y = -h / 2 + prad + regrow * pdiam;
        pieces[i]->cir.set(x, y, pdiam * 0.48);
    }
}

void tavli::Board::draw() const
{
    vvr::Shape::LineWidth = 8;
    canvas.draw();
}

/*-------------------------------------------------------------------------------------*/

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
