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

/*--- [Tavli scene] -------------------------------------------------------------------*/

TavliScene::TavliScene()
{
    m_bg_col = vvr::Colour("3d2001");
    mBoard = new tavli::Board();
    mPicker = new PiecePicker(mBoard->canvas, new tavli::PieceDragger2D);
}

TavliScene::~TavliScene()
{
    delete mPicker;
}

void TavliScene::reset()
{
    vvr::Scene::reset();
}

void TavliScene::resize()
{
    if (m_first_resize) 
    {
        mAxes = getGlobalAxes();
        mAxes->hide();
    }

    mBoard->resize(0.88*getViewportWidth(), 0.88*getViewportHeight());
}

void TavliScene::draw()
{
    enterPixelMode();
    mBoard->draw();
    mAxes->drawif();
    exitPixelMode();
}

void TavliScene::keyEvent(unsigned char key, bool up, int modif)
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

/*--- [Tavli entities] ----------------------------------------------------------------*/

void tavli::Piece::draw() const
{
    vvr::Circle2D::draw();
    vvr::Circle2D c(*this);
    c.SetRadius(c.GetRadius() * 0.70);
    c.colour.darker();
    c.draw();
}

void tavli::Piece::drop()
{

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
        pieces[i]->filled = true;
        pieces[i]->colour = vvr::Colour(0x95, 0, 0);
    }
}

void tavli::Board::resize(const float w, const float h)
{
    this->w = w;
    this->h = h;
    this->d = w / 13;
    this->r = d / 2;

    /* Passive drawing. Board terrain. */
    bounds[0]->set(-w / 2, -h / 2, -w / 2, +h / 2);
    bounds[1]->set(-w / 2, +h / 2, +w / 2, +h / 2);
    bounds[2]->set(+w / 2, +h / 2, +w / 2, -h / 2);
    bounds[3]->set(+w / 2, -h / 2, -w / 2, -h / 2);
    wood[0]->set(-r, -h / 2, -r, h / 2, r, +h / 2);
    wood[1]->set(-r, -h / 2, +r, h / 2, r, -h / 2);

    /* Pieces */
    for (size_t i = 0; i < pieces.size(); ++i) 
    {
        float x, y;
        size_t regcol = i % 6;
        size_t regrow = i / 6;
        x = d * (regcol + 1);
        y = -h / 2 + r + regrow * d;
        pieces[i]->set(C2DCircle({ x, y }, d * 0.48));
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
        return vvr::mainLoop(argc, argv, new TavliScene());
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}
