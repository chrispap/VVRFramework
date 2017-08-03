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
    mPicker = new PiecePicker(mBoard->getCanvas(), new tavli::PieceDragger2D);
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
    float mindist = 222222;
    Region* oldreg = region;
    Region* newreg = nullptr;

    for (auto &reg : board->regions) {
        float dist = fabs(reg->x - GetCentre().x);
        if (dist < mindist) {
            newreg = reg;
            mindist = dist;
        }
    }

    oldreg->removePiece(this);
    newreg->addPiece(this);
    newreg->resize();
    oldreg->resize();
}

void tavli::Region::draw() const
{
    float x = d * (regcol + 1);
    float ytop = 0;
    float ybot = -h / 2;
    float r = d / 2;

    vvr::Triangle2D t(x, ytop, x-r, ybot, x+r, ybot, vvr::darkGreen);
    t.filled = true;
    t.draw();
}

void tavli::Region::addPiece(Piece *piece)
{
    pieces.push_back(piece);
    piece->region = this;
}

void tavli::Region::removePiece(Piece *piece)
{
    pieces.erase(std::remove(pieces.begin(), pieces.end(), piece), pieces.end());
    piece->region = nullptr;
}

void tavli::Region::resize()
{
    float r = d / 2;

    for (size_t regrow = 0; regrow < pieces.size(); ++regrow)
    {
        float y = -h / 2 + r + regrow * d;
        pieces[regrow]->set(C2DCircle({ x, y }, d * 0.50));
    }
}

void tavli::Region::resize(float diam, float boardheight)
{
    d = diam;
    h = boardheight;
    x = d * (regcol + 1);
    resize();
}

tavli::Board::Board()
{
    /* Bounds */
    for (size_t i = 0; i < 4; ++i) {
        bounds.push_back(new vvr::LineSeg2D());
        canvas.add(bounds.back());
    }

    /* The wood in the middle */
    for (size_t i = 0; i < 2; ++i) {
        wood.push_back(new vvr::Triangle2D());
        wood.back()->filled = true;
        canvas.add(wood.back());
    }

    /* Regions */
    for (size_t i = 0; i < 6; ++i) {
        regions.push_back(new Region(i));
        canvas.add(regions.back());
    }

    /* Pieces */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = regions[i % 6];
        tavli::Piece* piece = new Piece(this);
        piece->filled = true;
        piece->colour = vvr::Colour(0x95, 0, 0);
        canvas.add(piece);
        reg->addPiece(piece);
    }
}

void tavli::Board::resize(const float width, const float height)
{
    w = width;
    h = height;
    d = width / 13;
    float r = d / 2;

    /* Passive drawing. Board terrain. */
    bounds[0]->set(-w / 2, -h / 2, -w / 2, +h / 2);
    bounds[1]->set(-w / 2, +h / 2, +w / 2, +h / 2);
    bounds[2]->set(+w / 2, +h / 2, +w / 2, -h / 2);
    bounds[3]->set(+w / 2, -h / 2, -w / 2, -h / 2);
    wood[0]->set(-r, -h / 2, -r, h / 2, r, +h / 2);
    wood[1]->set(-r, -h / 2, +r, h / 2, r, -h / 2);

    /* Pieces */
    for (auto &reg : regions) {
        reg->resize(d, h);
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
