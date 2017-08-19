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
using math::Ray;
using vvr::real;
using vvr::Colour;

/*---[tavli::scene]--------------------------------------------------------------------*/
static float HHH = 0.05f;
class TavliScene : public vvr::Scene
{
public:

    TavliScene(std::vector<Colour> &colours)
    {
        vvr_setmemb(colours);
        m_bg_col = Colour("222222");
        m_create_menus = false;
        m_perspective_proj = true;
        mBoard = nullptr;
        mPicker = nullptr;
        reset();
        mBoard->setupGamePortes();
    }

    ~TavliScene()
    {
        delete mPicker;
        delete mBoard;
    }

    const char* getName() const {
        return "Tavli Game";
    }

    void reset()
    {
        vvr::Scene::reset();
        delete mPicker;
        delete mBoard;
        mBoard = new tavli::Board(colours);
        mPicker = new tavli::PiecePicker(mBoard->canvas_pieces, new tavli::PieceDragger3D);
        vec pos = getFrustum().Pos();
        pos.y -= 55;
        pos.z += 25;
        setCameraPos(pos);
        resize();
    }

    void resize()
    {

        if (m_first_resize)
        {
            mAxes = getGlobalAxes();
            mAxes->hide();
        }

        const float w = 0.7 * getSceneWidth();
        const float h = 0.7 * getSceneWidth();
        mBoard->resize(w, h);
    }

    void draw()
    {
        mAxes->drawif();
        mBoard->drawif();
    }

    void keyEvent(unsigned char key, bool up, int modif)
    {
        switch (tolower(key))
        {
        case 'a': mAxes->toggleVisibility(); break;
        case 'b': mBoard->toggleVisibility(); break;
        case 'h': HHH -= 0.001f; vvr_echo(HHH); resize(); break;
        case 'j': HHH += 0.001f; vvr_echo(HHH); resize(); break;

        case '0':
        {
            vvr::Scene::reset();
            vec pos = getFrustum().Pos();
            pos.y -= 60;
            pos.z += 20;
            setCameraPos(pos);
            break;
        }

        case '1': mBoard->setupGamePortes(); break;
        case '2': mBoard->setupGamePlakwto(); break;
        case '3': mBoard->setupGameFevga(); break;

        default:
            vvr::Scene::keyEvent(key, up, modif);
        }
    }

    void mousePressed(int x, int y, int modif) override
    {
        if (ctrlDown(modif)) vvr::Scene::mousePressed(x, y, modif);
        mPicker->mousePressed(unproject(x, y), modif);
    }

    void mouseMoved(int x, int y, int modif) override
    {
        if (ctrlDown(modif)) vvr::Scene::mouseMoved(x, y, modif);
        mPicker->mouseMoved(unproject(x, y), modif);
    }

    void mouseReleased(int x, int y, int modif) override
    {
        if (ctrlDown(modif)) vvr::Scene::mouseReleased(x, y, modif);
        mPicker->mouseReleased(unproject(x, y), modif);
    }

private:
    vvr::Axes *mAxes;
    tavli::Board *mBoard;
    tavli::PiecePicker *mPicker;
    std::vector<Colour> colours;
};

/*---[tavli::Piece]--------------------------------------------------------------------*/

tavli::Piece::Piece(Colour col) : vvr::Cylinder3D(col)
{
    normal.Set(0, 0, 1);
}

void tavli::Piece::draw() const
{
    vvr::Cylinder3D::draw();
}

void tavli::Piece::pick()
{
    basecenter.z += height * 7;
}

void tavli::Piece::drop()
{
    float mindist = std::numeric_limits<float>::max();
    Region* oldreg = region;
    Region* newreg = nullptr;

    for (auto &reg : region->board->regions) {
        math::LineSegment l(reg->base, reg->top);
        l.a.z = 0;
        l.b.z = 0;
        float dist = l.Distance(basecenter);
        if (dist < mindist) {
            newreg = reg;
            mindist = dist;
        }
    }

    oldreg->removePiece(this);
    newreg->addPiece(this);
}

vvr::real tavli::Piece::pickdist(const math::Ray& ray) const
{
    math::Circle c(diskTop());
    float d;
    bool intersectsPlane = ray.Intersects(c.ContainingPlane(), &d);
    if (!intersectsPlane) return -1;
    float rd = ray.GetPoint(d).Distance(c.pos);
    float dist = ray.GetPoint(d).Distance(ray.pos);
    if (rd > radius) return -1;
    else return dist;
}

/*---[tavli::Region]-------------------------------------------------------------------*/

tavli::Region::Region(Board *board, int id, Colour col) : vvr::Triangle3D(col)
{
    vvr_setmemb(board);
    vvr_setmemb(id);
    rows = 5;
}

void tavli::Region::resize(float piecediam, float boardheight)
{
    vvr_setmemb(piecediam);
    vvr_setmemb(boardheight);

    float r = piecediam / 2;

    switch (id / 6)
    {
    case 0:
        base.x = piecediam * (6 - id);
        base.y = -boardheight / 2;
        top.y = -boardheight*0.05;
        dir.Set(0, 1, 0);
        break;
    case 1:
        base.x = -piecediam * (id - 5);
        base.y = -boardheight / 2;
        top.y = -boardheight*0.05;
        dir.Set(0, 1, 0);
        break;
    case 2:
        base.x = -piecediam * (18 - id);
        base.y = boardheight / 2;
        top.y = boardheight*0.05;
        dir.Set(0, -1, 0);
        break;
    case 3:
    case 4:
        base.x = piecediam * (id - 17);
        base.y = boardheight / 2;
        top.y = boardheight*0.05;
        dir.Set(0, -1, 0);
        break;
    default: assert(false);
    }

    if (id == 25) 
    {
        base.x = 0;
        base.y = -r * 1.5;
        base.z = boardheight * 0.08;
        top.x = base.x;
        top.y = -boardheight/2;
        top.z = base.z;
        dir.Set(0, -1, 0);
    }
    else if (id == 26) 
    {
        base.x = 0;
        base.y = r * 1.5;
        base.z = boardheight * 0.08;
        top.x = base.x;
        top.y = boardheight/2;
        top.z = base.z;
        dir.Set(0, 1, 0);
    }
    else
    {
        base.z = 0.001;
        top.x = base.x;
        top.z = base.z;
        a.Set(base.x, top.y, 0);
        b.Set(base.x - r, base.y, 0);
        c.Set(base.x + r, base.y, 0);
        if (id / 12) std::swap(b, c);
    }

    if (id == -1 || id == 24)
    {
        base.x += boardheight * 0.04;
        top.x += boardheight * 0.04;
    }

    arrangePieces();
}

void tavli::Region::arrangePieces(size_t index_from)
{
    float r = piecediam / 2;
    float h = r / 4;

    for (size_t i = index_from; i < pieces.size(); ++i)
    {
        pieces[i]->radius = r;
        pieces[i]->height = h;
        pieces[i]->basecenter = base + (dir*(piecediam*(i%rows) + r));
        pieces[i]->basecenter.z += (i / rows) * h * 1.1;
    }
}

void tavli::Region::addPiece(Piece *piece)
{
    pieces.push_back(piece);
    piece->region = this;
    arrangePieces(pieces.size() - 1);
}

void tavli::Region::removePiece(Piece *piece)
{
    pieces.erase(std::remove(pieces.begin(), pieces.end(), piece), pieces.end());
    piece->region = nullptr;
    arrangePieces();
}

/*---[tavli::Board]--------------------------------------------------------------------*/

tavli::Board::Board(std::vector<Colour> &colours)
{
    vvr_setmemb(colours);

    /* Regions (Normal) */
    for (int i = 0; i < 24; ++i) {
        regions.push_back(new Region(this, i, colours[1]));
        canvas_regions.add(regions.back());
    }

    /* Regions (Extra) */
    regions.push_back(new Region(this, -1, colours[1])); // [i=24]
    regions.push_back(new Region(this, 24, colours[1])); // [i=25]
    regions.push_back(new Region(this, 25, colours[1])); // [i=26]
    regions.push_back(new Region(this, 26, colours[1])); // [i=27]
    regions.at(24)->rows = 3;
    regions.at(25)->rows = 3;
    regions.at(26)->rows = 4;
    regions.at(27)->rows = 4;

    /* Load 3D model */
    mBoard3D = vvr::Mesh::Make(vvr::getBasePath() + "resources/obj/tavli.obj");
    mBoard3D->transform(math::float3x4::RotateX(math::pi / 2));
    mBoard3D->transform(math::float3x4::RotateZ(math::pi / 2));
    vec size = mBoard3D->getAABB().Size();
    mBoard3D->cornerAlign();
    mBoard3D->move({ 0,-size.y / 2, -size.z * 0.27f });

    vvr_msg("Created tavli board");
}

void tavli::Board::resize(const float width, const float height)
{
    vvr_setmemb(width);
    vvr_setmemb(height);

    /* Resize regions and they will also resize their pieces. */
    for (auto &reg : regions) {
        reg->resize(width / 13, height);
    }

    /* Resize 3D model */
    math::float3x4 mat;
    vec size, scale;
    mat.SetIdentity();
    mBoard3D->setTransform(mat);
    size = mBoard3D->getAABB().Size();
    scale = {
        width / size.x * 0.525f,
        height / size.y * 1.06f,
        3.00f
    };
    mat = mat * math::float3x4::Scale(scale);
    mBoard3D->setTransform(mat);
}

void tavli::Board::clearPieces()
{
    for (auto &reg : regions) {
        reg->pieces.clear();
    }

    canvas_pieces.clear();
}

void tavli::Board::setupGamePortes()
{
    clearPieces();

    static const int positions[24]
    {
        23,23,
        12,12,12,12,12,
        7,7,7,
        5,5,5,5,5
    };

    /* Pieces: Player 1 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = regions[positions[i]];
        tavli::Piece* piece = new Piece(colours[2]);
        reg->addPiece(piece);
        canvas_pieces.add(piece);
    }

    /* Pieces: Player 2 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = regions[23 - positions[i]];
        tavli::Piece* piece = new Piece(colours[3]);
        reg->addPiece(piece);
        canvas_pieces.add(piece);
    }
}

void tavli::Board::setupGamePlakwto()
{
    clearPieces();

    const int pl1reg = 0;
    const int pl2reg = 23;

    regions.at(pl1reg)->rows = 3;
    regions.at(pl2reg)->rows = 3;

    /* Pieces: Player 1 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = regions[pl1reg];
        tavli::Piece* piece = new Piece(colours[2]);
        reg->addPiece(piece);
        canvas_pieces.add(piece);
    }

    /* Pieces: Player 2 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = regions[pl2reg];
        tavli::Piece* piece = new Piece(colours[3]);
        reg->addPiece(piece);
        canvas_pieces.add(piece);
    }
}

void tavli::Board::setupGameFevga()
{
    clearPieces();

    const int pl1reg = 11;
    const int pl2reg = 11+12;

    regions.at(pl1reg)->rows = 3;
    regions.at(pl2reg)->rows = 3;

    /* Pieces: Player 1 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = regions[pl1reg];
        tavli::Piece* piece = new Piece(colours[2]);
        reg->addPiece(piece);
        canvas_pieces.add(piece);
    }

    /* Pieces: Player 2 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = regions[pl2reg];
        tavli::Piece* piece = new Piece(colours[3]);
        reg->addPiece(piece);
        canvas_pieces.add(piece);
    }
}

void tavli::Board::draw() const
{
    /* Draw pieces and regions */
    auto lw(vvr::Shape::LineWidth);
    canvas_regions.draw();
    canvas_pieces.draw();

    /* Draw 3D model */
    mBoard3D->draw(colours[0], vvr::SOLID);
    const auto t(mBoard3D->getTransform());
    auto tt = math::float3x4::RotateZ(math::pi) * t;
    mBoard3D->setTransform(tt);
    mBoard3D->draw(colours[0], vvr::SOLID);
    mBoard3D->setTransform(t);
}

/*---[tavli::PieceDragger3D]-----------------------------------------------------------*/

bool tavli::PieceDragger3D::grab(vvr::Drawable* drw)
{
    if (auto piece = dynamic_cast<tavli::Piece*>(drw))
    {
        col_org = piece->colour;
        piece->colour.mul(1.6);
        piece->pick();
        return true;
    }
    else return false;
}

void tavli::PieceDragger3D::drag(vvr::Drawable* drw, math::Ray ray0, math::Ray ray1)
{
    auto piece = static_cast<tavli::Piece*>(drw);
    float d0, d1;
    math::Plane boardplane(piece->diskBase().ContainingPlane());
    boardplane.Intersects(ray0, &d0);
    boardplane.Intersects(ray1, &d1);
    vec dv(ray1.GetPoint(d1) - ray0.GetPoint(d0));
    piece->basecenter += dv;
}

void tavli::PieceDragger3D::drop(vvr::Drawable* drw)
{
    auto piece = static_cast<tavli::Piece*>(drw);
    piece->colour = col_org;
    piece->drop();
}

/*---[main]----------------------------------------------------------------------------*/

int main(int argc, char* argv[])
{
    try
    {
        std::vector<Colour> colours;
        colours.resize(4);
        
        if (argc == 5) 
        {
            int coli = 0;
            colours[coli++] = Colour(argv[1]);
            colours[coli++] = Colour(argv[2]);
            colours[coli++] = Colour(argv[3]);
            colours[coli++] = Colour(argv[4]);
        }
        else 
        {
            int coli = 0;
            colours[coli++] = Colour("1A1111");
            colours[coli++] = Colour("222A00");
            colours[coli++] = Colour("700000");
            colours[coli++] = Colour("CCCCCC");
        }

        return vvr::mainLoop(argc, argv, new TavliScene(colours));
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}

/*-------------------------------------------------------------------------------------*/
