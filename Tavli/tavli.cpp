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

static float HHH = 0.10;
static float dHHH = 0.01;

using namespace vvr;
using namespace math;

/*---[tavli::scene]--------------------------------------------------------------------*/

class TavliScene : public Scene
{
public:
    TavliScene(std::vector<Colour> &colours)
    {
        vvr_setmemb(colours);
        m_bg_col = Colour("222222");
        m_create_menus = false;
        m_perspective_proj = true;
        m_fullscreen = false;
        mBoard = nullptr;
        reset();
    }

    ~TavliScene()
    {
        delete mBoard;
    }

    const char* getName() const  override
    {
        return "Tavli Game";
    }

    void reset() override
    {
        resetCamera();
        delete mBoard;
        mBoard = new tavli::Board(colours);
        mBoard->setupGamePortes();
        resize();
    }

    void resetCamera()
    {
        Scene::reset();
        vec pos = getFrustum().Pos();
        pos.y -= 55;
        pos.z += 25;
        setCameraPos(pos);
    }

    void resize() override
    {
        if (m_first_resize)
        {
            mAxes = getGlobalAxes();
            mAxes->hide();
        }

        const float width = 0.7 * getSceneWidth();
        const float height = 0.7 * getSceneWidth();
        mBoard->resize(width, height);
    }

    void draw() override
    {
        mAxes->drawif();
        mBoard->drawif();
    }

    void keyEvent(unsigned char key, bool up, int modif) override
    {
        switch (tolower(key))
        {
        case 'a': mAxes->toggleVisibility(); break;
        case 'b': mBoard->toggleVisibility(); break;
        case 'h': HHH -= dHHH; vvr_echo(HHH); resize(); break;
        case 'j': HHH += dHHH; vvr_echo(HHH); resize(); break;
        case '0': resetCamera(); break;
        case '1': mBoard->setupGamePortes(); break;
        case '2': mBoard->setupGamePlakwto(); break;
        case '3': mBoard->setupGameFevga(); break;
        default: Scene::keyEvent(key, up, modif);
        }
    }

    void mousePressed(int x, int y, int modif) override
    {
        if (ctrlDown(modif)) Scene::mousePressed(x, y, modif);
        mBoard->piecePicker->pick(unproject(x, y), modif);
    }

    void mouseMoved(int x, int y, int modif) override
    {
        if (ctrlDown(modif)) Scene::mouseMoved(x, y, modif);
        mBoard->piecePicker->move(unproject(x, y), modif);
    }

    void mouseReleased(int x, int y, int modif) override
    {
        if (ctrlDown(modif)) Scene::mouseReleased(x, y, modif);
        mBoard->piecePicker->drop(unproject(x, y), modif);
    }

private:
    Axes *mAxes;
    tavli::Board *mBoard;
    std::vector<Colour> colours;
};

/*---[tavli::Piece]--------------------------------------------------------------------*/

tavli::Piece::Piece(Colour col) : Cylinder3D(col)
{
    normal.Set(0, 0, 1);
}

void tavli::Piece::draw() const
{
    Cylinder3D::draw();
}

void tavli::Piece::pick()
{
    basecenter.z = region->boardheight * 0.10 + height;
}

real tavli::Piece::pickdist(const Ray& ray) const
{
    Circle c(diskTop());
    float d;
    bool intersectsPlane = ray.Intersects(c.ContainingPlane(), &d);
    if (!intersectsPlane) return -1;
    float rd = ray.GetPoint(d).Distance(c.pos);
    float dist = ray.GetPoint(d).Distance(ray.pos);
    if (rd > radius) return -1;
    else return dist;
}

/*---[tavli::Region]-------------------------------------------------------------------*/

tavli::Region::Region(Board *board, int id, Colour col) : Triangle3D(col)
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
        base.z = boardheight * 0.1f;
        top.x = base.x;
        top.y = -boardheight/2;
        top.z = base.z;
        dir.Set(0, -1, 0);
    }
    else if (id == 26)
    {
        base.x = 0;
        base.y = r * 1.5;
        base.z = boardheight * 0.1f;
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
    }

    a.Set(top.x, top.y, top.z);
    b.Set(base.x - r, base.y, base.z);
    c.Set(base.x + r, base.y, base.z);
    if ((id / 12 && id <= 24) || (id == 25)) {
        std::swap(b, c); // Fix CCW
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

real tavli::Region::pickdist(const Ray &ray) const
{
    LineSegment l(top, base);
    return l.Distance(ray);
}

/*---[tavli::Board]--------------------------------------------------------------------*/

tavli::Board::Board(std::vector<Colour> &colours)
{
    vvr_setmemb(colours);

    load3DModels();
    createRegions();

    /* Create picker objects */
    auto regionChooser = new RegionChooser();
    auto pieceDragger = new PieceDragger(regionCanvas, regionChooser);
    piecePicker = new PiecePicker(pieceCanvas, pieceDragger);

    vvr_msg("Created tavli board.");
}

tavli::Board::~Board()
{
    delete piecePicker;
    clearPieces();
    regionCanvas.setDelOnClear(false);
    regionCanvas.clear();
    for (auto &reg : regions) delete reg;

    vvr_msg("Deleted tavli board.");
}

void tavli::Board::load3DModels()
{
    std::string flnm = "tavli.obj";
    std::string path1 = getExePath() + flnm;
    std::string path2 = getBasePath() + "resources/obj/" + flnm;

    /* Load 3D model */
    try { if (!m3DBoard) m3DBoard = Mesh::Make(path1);
    } catch (std::string err) { vvr_msg(err); }

    try { if (!m3DBoard) m3DBoard = Mesh::Make(path2);
    } catch (std::string err) { vvr_msg(err); }

    if (!m3DBoard) {
        throw std::string("Could not find 3D model in any of the known locations.");
    }
    else {
        vvr_msg("Found 3D model.");
    }

    /* Fix positioning */
    m3DBoard->transform(float3x4::RotateX(pi / 2));
    m3DBoard->transform(float3x4::RotateZ(pi / 2));
    vec size = m3DBoard->getAABB().Size();
    m3DBoard->cornerAlign();
    m3DBoard->move({ 0,-size.y / 2, -size.z * 0.27f });
}

void tavli::Board::createRegions()
{
    /* Regions [Playing] */
    for (int i = 0; i < 24; ++i) {
        regions.push_back(new Region(this, i, colours[1]));
        regionCanvas.add(regions.back());
    }

    /* Regions [Plakomena] */
    regions.push_back(new Region(this, -1, colours[1])); // [i=24]
    regions.push_back(new Region(this, 24, colours[1])); // [i=25]
    regions.at(24)->rows = 3;
    regions.at(25)->rows = 3;

    /* Regions [Mazemena] */
    regions.push_back(new Region(this, 25, colours[1])); // [i=26]
    regions.push_back(new Region(this, 26, colours[1])); // [i=27]
    regions.at(26)->rows = 4;
    regions.at(27)->rows = 4;

    for (int i = 0; i < 4; i++) {
        regions.at(24 + i)->visible = false;
        regionCanvas.add(regions.at(24 + i));
    }
}

void tavli::Board::resize(const float width, const float height)
{
    vvr_setmemb(width);
    vvr_setmemb(height);

    /* Resize regions and they will also resize their pieces. */
    for (auto &reg : regions) {
        reg->resize(width / 13, height);
    }

    /* Resize 3D board */
    float3x4 mat;
    vec size, scale;
    mat.SetIdentity();
    m3DBoard->setTransform(mat);
    size = m3DBoard->getAABB().Size();
    scale = {
        width / size.x * 0.525f,
        height / size.y * 1.06f,
        width / 15
    };
    mat = mat * float3x4::Scale(scale);
    m3DBoard->setTransform(mat);
}

void tavli::Board::clearPieces()
{
    for (auto &reg : regions) reg->pieces.clear();
    pieceCanvas.clear();
}

void tavli::Board::setupGamePortes()
{
    clearPieces();

    static const int regs[15]
    {
        23,23,
        12,12,12,12,12,
        7,7,7,
        5,5,5,5,5
    };

    /* Pieces: Player 1 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = regions[regs[i]];
        Piece* piece = new Piece(colours[2]);
        reg->addPiece(piece);
        pieceCanvas.add(piece);
    }

    /* Pieces: Player 2 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = regions[23 - regs[i]];
        Piece* piece = new Piece(colours[3]);
        reg->addPiece(piece);
        pieceCanvas.add(piece);
    }

    vvr_msg("Setup game portes.");
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
        Piece* piece = new Piece(colours[2]);
        reg->addPiece(piece);
        pieceCanvas.add(piece);
    }

    /* Pieces: Player 2 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = regions[pl2reg];
        Piece* piece = new Piece(colours[3]);
        reg->addPiece(piece);
        pieceCanvas.add(piece);
    }

    vvr_msg("Setup game plakwto.");
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
        Piece* piece = new Piece(colours[2]);
        reg->addPiece(piece);
        pieceCanvas.add(piece);
    }

    /* Pieces: Player 2 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = regions[pl2reg];
        Piece* piece = new Piece(colours[3]);
        reg->addPiece(piece);
        pieceCanvas.add(piece);
    }

    vvr_msg("Setup game fevga.");
}

void tavli::Board::draw() const
{
    /* Draw pieces and regions */
    regionCanvas.draw();
    pieceCanvas.draw();

    /* Draw 3D model */
    m3DBoard->draw(colours[0], SOLID);
    const auto t(m3DBoard->getTransform());
    auto tt = float3x4::RotateZ(pi) * t;
    m3DBoard->setTransform(tt);
    m3DBoard->draw(colours[0], SOLID);
    m3DBoard->setTransform(t);
}

/*---[tavli::PieceDragger]-------------------------------------------------------------*/

bool tavli::PieceDragger::grab(Drawable* drw)
{
    if (auto piece = dynamic_cast<Piece*>(drw))
    {
        colour = piece->colour;
        piece->colour.mul(1.4);
        piece->pick();
        return true;
    }
    else return false;
}

void tavli::PieceDragger::drag(Drawable* drw, Ray ray0, Ray ray1)
{
    if (auto piece = dynamic_cast<Piece*>(drw))
    {
        float d0, d1;
        Plane boardplane(piece->diskBase().ContainingPlane());
        boardplane.Intersects(ray0, &d0);
        boardplane.Intersects(ray1, &d1);
        vec dv(ray1.GetPoint(d1) - ray0.GetPoint(d0));
        piece->basecenter += dv;
        regionPicker.drop(ray1, 0);
        regionPicker.pick(ray1, 0);
        ray = ray1;
    }
}

void tavli::PieceDragger::drop(Drawable* drw)
{
    assert(dynamic_cast<Piece*>(drw));
    auto piece = static_cast<Piece*>(drw);
    piece->colour = colour;
    Region* region = static_cast<Region*>(regionPicker.getDrawable());
    piece->region->removePiece(piece);
    region->addPiece(piece);
    regionPicker.drop(ray, 0);
}

/*---[tavli::RegionChooser]------------------------------------------------------------*/

bool tavli::RegionChooser::grab(Drawable* drw)
{
    assert(dynamic_cast<Region*>(drw));
    auto reg = static_cast<Region*>(drw);
    colour = reg->colour;
    visible = reg->visible;
    reg->colour.mul(1.4);
    reg->setColourPerVertex(reg->colour, reg->colour, reg->colour);
    reg->visible = true;
    return true;
}

void tavli::RegionChooser::drag(Drawable* drw, Ray ray0, Ray ray1)
{

}

void tavli::RegionChooser::drop(Drawable* drw)
{
    assert(dynamic_cast<Region*>(drw));
    auto reg = static_cast<Region*>(drw);
    reg->colour = colour;
    reg->setColourPerVertex(colour, colour, colour);
    reg->visible = visible;
}

/*---[main]----------------------------------------------------------------------------*/

int main(int argc, char* argv[])
{
    try
    {
        std::vector<Colour> colours;
        colours.resize(4);

        int coli = 0;

        /* Default colors */
        colours[coli++] = Colour("443322");
        colours[coli++] = Colour("242622");
        colours[coli++] = Colour("550000");
        colours[coli++] = Colour("BBBBBB");

#ifndef __APPLE__
        /* Load from CLI */
        for(coli=0; coli < argc-1; coli++) {
            colours[coli] = Colour(argv[1+coli]);
        }
#endif

        /* Print colours */
        vvr_msg("Colours: ");
        for (auto c : colours)
            printf("colours[coli++] = Colour(\"%02X%02X%02X\");\n", c.r, c.g, c.b);
        vvr_msg("\n");

        return mainLoop(argc, argv, new TavliScene(colours));
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}

/*-------------------------------------------------------------------------------------*/
