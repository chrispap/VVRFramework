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

TavliScene::TavliScene(vvr::Colour col1, vvr::Colour col2)
{
    vvr_setmemb(col1);
    vvr_setmemb(col2);
    m_bg_col = vvr::Colour("3d2001");
    m_perspective_proj = true;
    mBoard = nullptr;    
    mPicker = nullptr;
    reset();
}

TavliScene::~TavliScene()
{
    delete mPicker;
    delete mBoard;
}

void TavliScene::reset()
{
    vvr::Scene::reset();
    delete mPicker;
    delete mBoard;
    mBoard = new tavli::Board(col1, col2);
    mPicker = new tavli::PiecePicker(mBoard->getCanvas(), new tavli::PieceDragger);

    auto pos = getFrustum().Pos();
    pos.y -= 80;
    pos.z -= 20;
    setCameraPos(pos);

    resize();
}

void TavliScene::resize()
{
    if (m_first_resize) 
    {
        mAxes = getGlobalAxes();
        mAxes->hide();
    }

    float w = 0.8 * getSceneWidth();
    mBoard->resize(1.0 * w, 0.8 * w);
}

void TavliScene::draw()
{
    mBoard->draw();
    mAxes->drawif();
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

/*--- [Tavli Piece] -------------------------------------------------------------------*/

tavli::Piece::Piece(Board* board, vvr::Colour col) : vvr::Cylinder3D(col), board(board)
{
    normal.Set(0, 0, 1);
}

void tavli::Piece::draw() const
{
    vvr::Cylinder3D::draw();
}

void tavli::Piece::drop()
{
    float mindist = std::numeric_limits<float>::max();
    Region* oldreg = region;
    Region* newreg = nullptr;

    for (auto &reg : board->regions) {
        math::LineSegment l(reg->base, reg->top);
        float dist = l.Distance(basecenter);
        if (dist < mindist) {
            newreg = reg;
            mindist = dist;
        }
    }

    oldreg->removePiece(this);
    newreg->addPiece(this);
    newreg->arrangePieces();
    oldreg->arrangePieces();
}

/*--- [Tavli Region] ------------------------------------------------------------------*/

tavli::Region::Region(int regcol) : vvr::Triangle3D(vvr::darkGreen), regcol(regcol) 
{

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

void tavli::Region::resize(float piecediam, float boardheight)
{
    vvr_setmemb(piecediam);
    vvr_setmemb(boardheight);
    base.z = 0.001;
    float r = piecediam / 2;
    float ytop;

    switch (regcol / 6)
    {
    case 0:
        base.x = piecediam * (6 - regcol);
        base.y = -boardheight / 2;
        ytop = -boardheight*0.05;
        updir.Set(0, 1, 0);
        break;
    case 1:
        base.x = -piecediam * (regcol - 5);
        base.y = -boardheight / 2;
        ytop = -boardheight*0.05;
        updir.Set(0, 1, 0);
        break;
    case 2:
        base.x = -piecediam * (18 - regcol);
        base.y = boardheight / 2;
        ytop = boardheight*0.05;
        updir.Set(0, -1, 0);
        break;
    case 3:
        base.x = piecediam * (regcol - 17);
        base.y = boardheight / 2;
        ytop = boardheight*0.05;
        updir.Set(0, -1, 0);
        break;
    default:
        assert(false);
    }

    top = base;
    top.y = ytop;

    a.Set(base.x, ytop, 0);
    b.Set(base.x - r, base.y, 0);
    c.Set(base.x + r, base.y, 0);
    if (regcol / 12) std::swap(a, b); // Fix CCW of opposite sides.

    arrangePieces();
}

void tavli::Region::arrangePieces()
{
    float r = piecediam / 2;

    for (size_t i = 0; i < pieces.size(); ++i)
    {
        pieces[i]->basecenter = base + (updir*(piecediam*i+r));
        pieces[i]->radius = r;
        pieces[i]->height = r / 4;
    }
}

/*--- [Tavli Board] -------------------------------------------------------------------*/

tavli::Board::Board(vvr::Colour col1, vvr::Colour col2)
{
    /* The wood in the middle */
    for (size_t i = 0; i < 2; ++i) {
        wood.push_back(new vvr::Triangle2D());
        wood.back()->filled = true;
        canvas.add(wood.back());
    }

    /* Regions */
    for (size_t i = 0; i < 24; ++i) {
        regions.push_back(new Region(i));
        canvas.add(regions.back());
    }

    /* Pieces */
    int positions_plakwto[24]
    {
        23,23,
        12,12,12,12,12,
        7,7,7,
        5,5,5,5,5
    };

    /* Player 1 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = regions[positions_plakwto[i]];
        tavli::Piece* piece = new Piece(this, col1);
        reg->addPiece(piece);
        canvas.add(piece);
    }

    /* Player 2 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = regions[23-positions_plakwto[i]];
        tavli::Piece* piece = new Piece(this, col2);
        reg->addPiece(piece);
        canvas.add(piece);
    }

    /* Bounds */
    for (size_t i = 0; i < 4; ++i) {
        bounds.push_back(new vvr::LineSeg2D());
        canvas.add(bounds.back());
    }
}

void tavli::Board::resize(const float w, const float h)
{
    width = w;
    height = h;
    float d = width / 13;
    float r = d / 2;

    /* Reszie board terrain. (Passive drawing) */
    bounds[0]->set(-w / 2, -h / 2, -w / 2, +h / 2);
    bounds[1]->set(-w / 2, +h / 2, +w / 2, +h / 2);
    bounds[2]->set(+w / 2, +h / 2, +w / 2, -h / 2);
    bounds[3]->set(+w / 2, -h / 2, -w / 2, -h / 2);
    wood[0]->set(-r, -h / 2, -r, h / 2, r, +h / 2);
    wood[1]->set(-r, -h / 2, +r, h / 2, r, -h / 2);

    /* Regions. (Will also resize their pieces) */
    for (auto &reg : regions) {
        reg->resize(d, h);
    }
}

void tavli::Board::draw() const
{
    auto lw(vvr::Shape::LineWidth);
    vvr::Shape::LineWidth = 12;
    canvas.draw();
    vvr::Shape::LineWidth = lw;
}

/*-------------------------------------------------------------------------------------*/

int main(int argc, char* argv[])
{
    try
    {
        vvr::Colour col1("950000");
        vvr::Colour col2("DDDDDD");
        
        if (argc == 3) {
            col1 = vvr::Colour(argv[1]);
            col2 = vvr::Colour(argv[2]);
        }

        return vvr::mainLoop(argc, argv, new TavliScene(col1, col2));
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}
