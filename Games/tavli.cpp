#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/palette.h>
#include <vvr/picking.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <set>

static float HHH = 0.10;
static float dHHH = 0.01;

/*---[Declarations]---------------------------------------------------------------------*/
namespace tavli
{
    using namespace vvr;

    struct RegionHighlighter;
    struct PieceDragger;
    struct Piece;
    struct Region;
    class  Board;

    //! Pickers
    typedef MousePicker3D<RegionHighlighter> RegionPicker;
    typedef MousePicker3D<PieceDragger> PiecePicker;

    //! Draggers
    struct RegionHighlighter
    {
        vvr_decl_shared_ptr(RegionHighlighter)

        bool on_pick(vvr::Drawable* drw, Ray ray);
        void on_drag(vvr::Drawable* drw, Ray ray0, Ray ray1) {}
        void on_drop(vvr::Drawable* drw);

    private:
        vvr::Colour _colour;
    };

    struct PieceDragger
    {
        vvr_decl_shared_ptr(PieceDragger)

        bool on_pick(vvr::Drawable* drw, Ray ray);
        void on_drag(vvr::Drawable* drw, Ray ray0, Ray ray1);
        void on_drop(vvr::Drawable* drw);

        PieceDragger(RegionPicker* rp) : _regionPicker{ rp } {}

    private:
        Ray             _ray;
        vvr::Colour     _colour;
        RegionPicker*   _regionPicker;
    };

    //! Game entities
    struct Piece : public Cylinder3D
    {
        Piece(vvr::Colour col);
        void draw() const override;
        real pickdist(const Ray& ray) const override;
        void on_pick();

    public:
        Region* region;
    };

    struct Region : public Triangle3D
    {
        Region(Board* board, int reg, vvr::Colour colour);
        void addPiece(Piece *piece);
        void removePiece(Piece *piece);
        void resize(float diam, float boardheight);
        void arrangePieces(size_t index_from=0);
        real pickdist(const Ray& ray) const override;

    public:
        Board* board;
        std::vector<Piece*> pieces;
        float piecediam, boardheight;
        int id, rows;
        vec base, top, dir;
    };

    struct Board : public vvr::Drawable
    {
        Board(std::vector<vvr::Colour> &cols);
        ~Board();
        void load3DModels();
        void createRegions();
        void clearPieces();
        void setupGamePortes();
        void setupGamePlakwto();
        void setupGameFevga();
        void draw() const override;
        void resize(const float width, const float height);

    public:
        RegionHighlighter::Ptr  region_hlter;
        RegionPicker::Ptr       region_picker;
        PieceDragger::Ptr       piece_dragger;
        PiecePicker::Ptr        piece_picker;

    private:
        std::vector<vvr::Colour> _colours;
        std::vector<Piece*>     _pieces;
        std::vector<Region*>    _regions;
        Mesh::Ptr               _3DBoard;
        Canvas                  _pieceCanvas;
        Canvas                  _regionCanvas;
        float                   _width;
        float                   _height;
    };
}

class TavliScene : public vvr::Scene
{
public:
    TavliScene(const std::vector<vvr::Colour> &colours);
    ~TavliScene();

    const char* getName() const  override {
        return "Tavli Game";
    }

    void reset() override;
    void resetCamera();
    void resize() override;
    void draw() override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseHovered(int x, int y, int modif) override;

private:
    vvr::Axes*                  _axes;
    tavli::Board*               _board;
    std::vector<vvr::Colour>    _colours;
};

/*---[TavliScene]-----------------------------------------------------------------------*/
TavliScene::TavliScene(const std::vector<vvr::Colour> &colours)
{
    m_bg_col = vvr::Colour("222222");
    m_create_menus = true;
    m_show_log = true;
    m_perspective_proj = true;
    m_fullscreen = false;
    _colours = colours;
    _board = nullptr;
    reset();
}

TavliScene::~TavliScene()
{
    delete _board;
}

void TavliScene::reset()
{
    resetCamera();
    delete _board;
    _board = new tavli::Board(_colours);
    _board->setupGamePortes();
    resize();
}

void TavliScene::resetCamera()
{
    Scene::reset();
    vec pos = getFrustum().Pos();
    pos.y -= 55;
    pos.z += 25;
    setCameraPos(pos);
}

void TavliScene::resize()
{
    if (m_first_resize)
    {
        _axes = &getGlobalAxes();
        _axes->hide();
    }

    const float width = 0.7 * getSceneWidth();
    const float height = 0.7 * getSceneWidth();
    _board->resize(width, height);
}

void TavliScene::draw()
{
    _axes->drawif();
    _board->drawif();
}

void TavliScene::keyEvent(unsigned char key, bool up, int modif)
{
    switch (tolower(key))
    {
    case 'a': _axes->toggleVisibility(); break;
    case 'b': _board->toggleVisibility(); break;
    case 'h': HHH -= dHHH; vvr_echo(HHH); resize(); break;
    case 'j': HHH += dHHH; vvr_echo(HHH); resize(); break;
    case '0': resetCamera(); break;
    case '1': _board->setupGamePortes(); break;
    case '2': _board->setupGamePlakwto(); break;
    case '3': _board->setupGameFevga(); break;
    default: Scene::keyEvent(key, up, modif);
    }
}

void TavliScene::mousePressed(int x, int y, int modif)
{
    _board->piece_picker->do_pick(unproject(x, y), modif);
    if (_board->piece_picker->picked()) {
        cursorGrab();
    } else Scene::mousePressed(x, y, modif);
}

void TavliScene::mouseMoved(int x, int y, int modif)
{
    if (_board->piece_picker->picked()) {
        _board->piece_picker->do_drag(unproject(x, y), modif);
    } else Scene::mouseMoved(x, y, modif);
}

void TavliScene::mouseReleased(int x, int y, int modif)
{
    _board->piece_picker->do_drop(unproject(x, y), modif);
    cursorShow();
}

void TavliScene::mouseHovered(int x, int y, int modif)
{
    _board->region_picker->do_pick(unproject(x, y), modif);
    if (_board->region_picker->picked()) {
        cursorHand();
    } else cursorShow();
}

/*---[tavli::Board]---------------------------------------------------------------------*/
tavli::Board::Board(std::vector<vvr::Colour> &colours)
{
    _colours = colours;
    load3DModels();
    createRegions();
    region_hlter = RegionHighlighter::Make();
    region_picker = RegionPicker::Make(_regionCanvas, region_hlter.get());
    piece_dragger = PieceDragger::Make(region_picker.get());
    piece_picker = PiecePicker::Make(_pieceCanvas, piece_dragger.get());
    vvr_msg("Created tavli board.");
}

tavli::Board::~Board()
{
    clearPieces();
    _regionCanvas.setDelOnClear(false);
    _regionCanvas.clear();
    for (auto &reg : _regions) delete reg;
    vvr_msg("Deleted tavli board.");
}

void tavli::Board::load3DModels()
{
    std::string flnm = "tavli.obj";
    std::string path1 = get_exe_path() + flnm;
    std::string path2 = get_base_path() + "resources/obj/" + flnm;

    /* Load 3D model */
    try {
        if (!_3DBoard) _3DBoard = Mesh::Make(path1);
    }
    catch (std::string err) { vvr_msg(err); }

    try {
        if (!_3DBoard) _3DBoard = Mesh::Make(path2);
    }
    catch (std::string err) { vvr_msg(err); }

    if (!_3DBoard) {
        throw std::string("Could not find 3D model in any of the known locations.");
    }
    else {
        vvr_msg("Found 3D model.");
    }

    /* Fix positioning */
    _3DBoard->transform(float3x4::RotateX(pi / 2));
    _3DBoard->transform(float3x4::RotateZ(pi / 2));
    vec size = _3DBoard->getAABB().Size();
    _3DBoard->cornerAlign();
    _3DBoard->move({ 0,-size.y / 2, -size.z * 0.27f });
}

void tavli::Board::createRegions()
{
    /* Regions [Playing] */
    for (int i = 0; i < 24; ++i) {
        _regions.push_back(new Region(this, i, _colours[1]));
        _regionCanvas.add(_regions.back());
    }

    /* Regions [Plakomena] */
    _regions.push_back(new Region(this, -1, _colours[1])); // [i=24]
    _regions.push_back(new Region(this, 24, _colours[1])); // [i=25]
    _regions.at(24)->rows = 3;
    _regions.at(25)->rows = 3;

    /* Regions [Mazemena] */
    _regions.push_back(new Region(this, 25, _colours[1])); // [i=26]
    _regions.push_back(new Region(this, 26, _colours[1])); // [i=27]
    _regions.at(26)->rows = 4;
    _regions.at(27)->rows = 4;

    for (int i = 0; i < 4; i++) {
        _regions.at(24 + i)->visible = false;
        _regionCanvas.add(_regions.at(24 + i));
    }
}

void tavli::Board::resize(const float width, const float height)
{
    _width = width;
    _height = height;

    /* Resize regions and they will also resize their pieces. */
    for (auto &reg : _regions) {
        reg->resize(width / 13, height);
    }

    /* Resize 3D board */
    float3x4 mat;
    vec size, scale;
    mat.SetIdentity();
    _3DBoard->setTransform(mat);
    size = _3DBoard->getAABB().Size();
    scale = {
        width / size.x * 0.525f,
        height / size.y * 1.06f,
        width / 15
    };
    mat = mat * float3x4::Scale(scale);
    _3DBoard->setTransform(mat);
}

void tavli::Board::clearPieces()
{
    for (auto &reg : _regions) reg->pieces.clear();
    _pieceCanvas.clear();
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
        Region* reg = _regions[regs[i]];
        Piece* piece = new Piece(_colours[2]);
        reg->addPiece(piece);
        _pieceCanvas.add(piece);
    }

    /* Pieces: Player 2 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = _regions[23 - regs[i]];
        Piece* piece = new Piece(_colours[3]);
        reg->addPiece(piece);
        _pieceCanvas.add(piece);
    }

    vvr_msg("Setup game portes.");
}

void tavli::Board::setupGamePlakwto()
{
    clearPieces();

    const int pl1reg = 0;
    const int pl2reg = 23;

    _regions.at(pl1reg)->rows = 3;
    _regions.at(pl2reg)->rows = 3;

    /* Pieces: Player 1 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = _regions[pl1reg];
        Piece* piece = new Piece(_colours[2]);
        reg->addPiece(piece);
        _pieceCanvas.add(piece);
    }

    /* Pieces: Player 2 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = _regions[pl2reg];
        Piece* piece = new Piece(_colours[3]);
        reg->addPiece(piece);
        _pieceCanvas.add(piece);
    }

    vvr_msg("Setup game plakwto.");
}

void tavli::Board::setupGameFevga()
{
    clearPieces();

    const int pl1reg = 11;
    const int pl2reg = 11 + 12;

    _regions.at(pl1reg)->rows = 3;
    _regions.at(pl2reg)->rows = 3;

    /* Pieces: Player 1 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = _regions[pl1reg];
        Piece* piece = new Piece(_colours[2]);
        reg->addPiece(piece);
        _pieceCanvas.add(piece);
    }

    /* Pieces: Player 2 */
    for (size_t i = 0; i < 15; ++i) {
        Region* reg = _regions[pl2reg];
        Piece* piece = new Piece(_colours[3]);
        reg->addPiece(piece);
        _pieceCanvas.add(piece);
    }

    vvr_msg("Setup game fevga.");
}

void tavli::Board::draw() const
{
    /* Draw pieces and regions */
    _regionCanvas.draw();
    _pieceCanvas.draw();

    /* Draw 3D model */
    _3DBoard->draw(_colours[0], SOLID);
    const auto t(_3DBoard->getTransform());
    auto tt = float3x4::RotateZ(pi) * t;
    _3DBoard->setTransform(tt);
    _3DBoard->draw(_colours[0], SOLID);
    _3DBoard->setTransform(t);
}

/*---[tavli::Piece]---------------------------------------------------------------------*/
tavli::Piece::Piece(vvr::Colour col) : Cylinder3D(col)
{
    normal.Set(0, 0, 1);
}

void tavli::Piece::draw() const
{
    Cylinder3D::draw();
}

void tavli::Piece::on_pick()
{
    basecenter.z = region->boardheight * 0.10 + height;
}

vvr::real tavli::Piece::pickdist(const Ray& ray) const
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

/*---[tavli::Region]--------------------------------------------------------------------*/
tavli::Region::Region(Board *board, int id, vvr::Colour col) : Triangle3D(col)
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

vvr::real tavli::Region::pickdist(const Ray &ray) const
{
    LineSegment l(top, base);
    auto d = l.Distance(ray);
    return (d < piecediam) ? d : -1;
}

/*---[tavli::PieceDragger]--------------------------------------------------------------*/
bool tavli::PieceDragger::on_pick(vvr::Drawable* drw, Ray ray)
{
    assert(typeid(Piece)==typeid(*drw));
    auto piece = static_cast<Piece*>(drw);
    _colour = piece->colour;
    piece->colour.mul(1.4);
    piece->on_pick();
    return true;
}

void tavli::PieceDragger::on_drag(vvr::Drawable* drw, Ray ray0, Ray ray1)
{
    assert(dynamic_cast<Piece*>(drw));
    auto piece = static_cast<Piece*>(drw);
    float d0, d1;
    Plane boardplane(piece->diskBase().ContainingPlane());
    boardplane.Intersects(ray0, &d0);
    boardplane.Intersects(ray1, &d1);
    vec dv(ray1.GetPoint(d1) - ray0.GetPoint(d0));
    piece->basecenter += dv;
    _regionPicker->do_drop(ray1, 0);
    _regionPicker->do_pick(ray1, 0);
    _ray = ray1;
}

void tavli::PieceDragger::on_drop(vvr::Drawable* drw)
{
    auto piece = static_cast<Piece*>(drw);
    auto region = static_cast<Region*>(_regionPicker->picked());
    if (!region) region = piece->region;
    piece->colour = _colour;
    piece->region->removePiece(piece);
    region->addPiece(piece);
    _regionPicker->do_drop(_ray, 0);
}

/*---[tavli::RegionHighlighter]---------------------------------------------------------*/
bool tavli::RegionHighlighter::on_pick(vvr::Drawable* drw, Ray ray)
{
    assert(typeid(Region)==typeid(*drw));
    auto reg = static_cast<Region*>(drw);
    _colour = reg->colour;
    reg->colour.mul(1.50);
    reg->setColourPerVertex(reg->colour, reg->colour, reg->colour);
    return true;
}

void tavli::RegionHighlighter::on_drop(vvr::Drawable* drw)
{
    auto reg = static_cast<Region*>(drw);
    reg->colour = _colour;
    reg->setColourPerVertex(_colour, _colour, _colour);
}

/*---[main]-----------------------------------------------------------------------------*/
namespace tavli {
    std::vector<vvr::Colour> GetDefaultColours()
    {
        std::vector<vvr::Colour> colours = {
            vvr::Colour(0x443322),  ///> Board
            vvr::Colour(0x242622),  ///> Regions
            vvr::Colour(0x550000),  ///> Team 1
            vvr::Colour(0xBBBBBB)   ///> Team 2
        };
        return colours;
    }
}

#ifndef ALL_DEMO_APP
int main(int argc, char* argv[])
{
    try
    {
        std::vector<vvr::Colour> colours = tavli::GetDefaultColours();
#ifndef __APPLE__
        /* Load from CLI */
        for(int coli=0; coli < argc-1; coli++) {
            colours[coli] = vvr::Colour(argv[1+coli]);
        }
#endif

        /* Print colours */
        vvr_msg("Colours: ");
        for (auto c : colours)
            printf("colours[coli++] = vvr::Colour(\"%02X%02X%02X\");\n", c.r, c.g, c.b);
        vvr_msg("\n");

        return main_with_scene(argc, argv, new TavliScene(colours));
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}
#endif
/*--------------------------------------------------------------------------------------*/
