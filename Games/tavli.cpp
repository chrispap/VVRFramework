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

static float HHH = 0.10f;
static float dHHH = 0.01f;

/*---[Declarations]---------------------------------------------------------------------*/
namespace tavli
{
    using namespace vvr;

    struct RegionHighlighter;
    struct PieceDragger;
    struct Piece;
    struct Region;
    struct Board;

    //! Pickers
    typedef MousePicker3D<RegionHighlighter> RegionPicker;
    typedef MousePicker3D<PieceDragger> PiecePicker;

    //! Draggers
    struct RegionHighlighter
    {
        vvr_decl_shared_ptr(RegionHighlighter)

        bool on_pick(vvr::Drawable* drw, Ray ray);
        void on_drop(vvr::Drawable* drw);
        void on_drag(vvr::Drawable*, Ray, Ray) {}

    private:
        vvr::Colour colour;
    };

    struct PieceDragger
    {
        vvr_decl_shared_ptr(PieceDragger)

        bool on_pick(vvr::Drawable* drw, Ray ray);
        void on_drag(vvr::Drawable* drw, Ray ray0, Ray ray1);
        void on_drop(vvr::Drawable* drw);

        PieceDragger(RegionPicker* rp) : regionPicker{ rp } {}

    private:
        Ray             ray;
        vvr::Colour     colour;
        RegionPicker*   regionPicker;
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
        Region(Board* board, int id, vvr::Colour colour);
        void addPiece(Piece *piece);
        void removePiece(Piece *piece);
        void resize(float diam, float boardheight);
        void arrangePieces(size_t index_from=0);
        real pickdist(const Ray& ray) const override;

    public:
        Board* board;
        std::vector<Piece*> pieces;
        float piecediam, boardheight;
        vec base, top, dir;
        size_t rows;
        int id;
    };

    struct Board : public vvr::Drawable
    {
        Board(std::vector<vvr::Colour> &cols);
        ~Board() override;
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
        std::vector<vvr::Colour> colours;
        std::vector<Piece*>     pieces;
        std::vector<Region*>    regions;
        Mesh::Ptr               boardMesh;
        Canvas                  pieceCanvas;
        Canvas                  regionCanvas;
        float                   width;
        float                   height;
    };
}

class TavliScene : public vvr::Scene
{
public:
    TavliScene(const std::vector<vvr::Colour> &colours);
    ~TavliScene() override;

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
    vvr::Axes*                  axes;
    tavli::Board*               board;
    std::vector<vvr::Colour>    colours;
};

/*---[TavliScene]-----------------------------------------------------------------------*/
TavliScene::TavliScene(const std::vector<vvr::Colour> &colours)
{
    this->colours = colours;
    m_bg_col = vvr::Colour("222222");
    m_create_menus = false;
    m_show_log = true;
    m_perspective_proj = true;
    m_fullscreen = false;
    board = nullptr;
    reset();
}

TavliScene::~TavliScene()
{
    delete board;
}

void TavliScene::reset()
{
    resetCamera();
    delete board;
    board = new tavli::Board(colours);
    board->setupGamePortes();
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
        axes = &getGlobalAxes();
        axes->hide();
    }

    const float width = 0.7f * getSceneWidth();
    const float height = 0.7f * getSceneWidth();
    board->resize(width, height);
}

void TavliScene::draw()
{
    axes->drawif();
    board->drawif();
}

void TavliScene::keyEvent(unsigned char key, bool up, int modif)
{
    switch (tolower(key))
    {
    case 'a': axes->toggle(); break;
    case 'b': board->toggle(); break;
    case 'h': HHH -= dHHH; vvr_echo(HHH); resize(); break;
    case 'j': HHH += dHHH; vvr_echo(HHH); resize(); break;
    case '0': resetCamera(); break;
    case '1': board->setupGamePortes(); break;
    case '2': board->setupGamePlakwto(); break;
    case '3': board->setupGameFevga(); break;
    default: Scene::keyEvent(key, up, modif);
    }
}

void TavliScene::mousePressed(int x, int y, int modif)
{
    board->piece_picker->do_pick(unproject(x, y), modif);
    if (board->piece_picker->picked()) {
        cursorGrab();
    } else Scene::mousePressed(x, y, modif);
}

void TavliScene::mouseMoved(int x, int y, int modif)
{
    if (board->piece_picker->picked()) {
        board->piece_picker->do_drag(unproject(x, y), modif);
    } else if (this->altDown(modif)) Scene::mouseMoved(x, y, modif);
}

void TavliScene::mouseReleased(int x, int y, int modif)
{
    board->piece_picker->do_drop(unproject(x, y), modif);
    cursorShow();
}

void TavliScene::mouseHovered(int x, int y, int modif)
{
    //_board->region_picker->do_pick(unproject(x, y), modif);
    board->piece_picker->do_pick(unproject(x, y), modif);
    if (board->piece_picker->picked()) {
        //auto piece = static_cast<tavli::Piece*>(_board->piece_picker->picked());
        cursorHand();
    } else cursorShow();
}

/*---[tavli::Board]---------------------------------------------------------------------*/
tavli::Board::Board(std::vector<vvr::Colour> &colours)
{
    this->colours = colours;
    load3DModels();
    createRegions();
    region_hlter = RegionHighlighter::Make();
    region_picker = RegionPicker::Make(regionCanvas, region_hlter.get());
    piece_dragger = PieceDragger::Make(region_picker.get());
    piece_picker = PiecePicker::Make(pieceCanvas, piece_dragger.get());
    vvr_msg("Created tavli board.");
}

tavli::Board::~Board()
{
    clearPieces();
    regionCanvas.setDelOnClear(false);
    regionCanvas.clear();
    for (auto &reg : regions) delete reg;
    vvr_msg("Deleted tavli board.");
}

void tavli::Board::load3DModels()
{
    std::string flnm = "tavli.obj";
    std::string path1 = get_exe_path() + flnm;
    std::string path2 = get_base_path() + "resources/obj/" + flnm;

    /* Load 3D model */
    try {
        if (!boardMesh) boardMesh = Mesh::Make(path1);
    }
    catch (std::string err) { vvr_msg(err); }

    try {
        if (!boardMesh) boardMesh = Mesh::Make(path2);
    }
    catch (std::string err) { vvr_msg(err); }

    if (!boardMesh) {
        throw std::string("Could not find 3D model in any of the known locations.");
    }
    else {
        vvr_msg("Found 3D model.");
    }

    /* Fix positioning */
    boardMesh->transform(float3x4::RotateX(pi / 2));
    boardMesh->transform(float3x4::RotateZ(pi / 2));
    vec size = boardMesh->getAABB().Size();
    boardMesh->cornerAlign();
    boardMesh->move({ 0,-size.y / 2, -size.z * 0.27f });
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

    for (size_t i = 0; i < 4; i++) {
        regions.at(24 + i)->visible = false;
        regionCanvas.add(regions.at(24 + i));
    }
}

void tavli::Board::resize(const float width, const float height)
{
    this->width = width;
    this->height = height;

    /* Resize regions and they will also resize their pieces. */
    for (auto &reg : regions) {
        reg->resize(width / 13, height);
    }

    /* Resize 3D board */
    float3x4 mat;
    vec size, scale;
    mat.SetIdentity();
    boardMesh->setTransform(mat);
    size = boardMesh->getAABB().Size();
    scale = {
        width / size.x * 0.525f,
        height / size.y * 1.06f,
        width / 15
    };
    mat = mat * float3x4::Scale(scale);
    boardMesh->setTransform(mat);
}

void tavli::Board::clearPieces()
{
    for (auto &reg : regions) reg->pieces.clear();
    pieceCanvas.clear();
}

void tavli::Board::setupGamePortes()
{
    clearPieces();

    regions.at(11)->rows = 5;
    regions.at(11+12)->rows = 5;

    static const size_t regs[15]
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

    regions.at(11)->rows = 5;
    regions.at(11+12)->rows = 5;

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
    const int pl2reg = 11 + 12;

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
    boardMesh->draw(colours[0], SOLID);
    const auto t(boardMesh->getTransform());
    auto tt = float3x4::RotateZ(pi) * t;
    boardMesh->setTransform(tt);
    boardMesh->draw(colours[0], SOLID);
    boardMesh->setTransform(t);
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
    basecenter.z = region->boardheight * 0.10f + height;
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
    default:
    case 0:
        base.x = piecediam * (6 - id);
        base.y = -boardheight / 2;
        top.y = -boardheight * 0.05f;
        dir.Set(0, 1, 0);
        break;
    case 1:
        base.x = -piecediam * (id - 5);
        base.y = -boardheight / 2;
        top.y = -boardheight * 0.05f;
        dir.Set(0, 1, 0);
        break;
    case 2:
        base.x = -piecediam * (18 - id);
        base.y = boardheight / 2;
        top.y = boardheight * 0.05f;
        dir.Set(0, -1, 0);
        break;
    case 3:
    case 4:
        base.x = piecediam * (id - 17);
        base.y = boardheight / 2;
        top.y = boardheight * 0.05f;
        dir.Set(0, -1, 0);
        break;
    }

    if (id == 25)
    {
        base.x = 0;
        base.y = -r * 1.5f;
        base.z = boardheight * 0.1f;
        top.x = base.x;
        top.y = -boardheight/2;
        top.z = base.z;
        dir.Set(0, -1, 0);
    }
    else if (id == 26)
    {
        base.x = 0;
        base.y = r * 1.5f;
        base.z = boardheight * 0.1f;
        top.x = base.x;
        top.y = boardheight/2;
        top.z = base.z;
        dir.Set(0, 1, 0);
    }
    else
    {
        base.z = 0.001f;
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
        base.x += boardheight * 0.04f;
        top.x += boardheight * 0.04f;
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
        pieces[i]->basecenter.z += (i / rows) * h * 1.1f;
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
bool tavli::PieceDragger::on_pick(vvr::Drawable* drw, Ray)
{
    assert(typeid(Piece)==typeid(*drw));
    auto piece = static_cast<Piece*>(drw);
    colour = piece->colour;
    piece->colour.mul(1.4f);
    return true;
}

void tavli::PieceDragger::on_drag(vvr::Drawable* drw, Ray ray0, Ray ray1)
{
    assert(dynamic_cast<Piece*>(drw));
    auto piece = static_cast<Piece*>(drw);
    float d0, d1;
    piece->on_pick();
    Plane boardplane(piece->diskBase().ContainingPlane());
    boardplane.Intersects(ray0, &d0);
    boardplane.Intersects(ray1, &d1);
    vec dv(ray1.GetPoint(d1) - ray0.GetPoint(d0));
    piece->basecenter += dv;
    regionPicker->do_drop(ray1, 0);
    regionPicker->do_pick(ray1, 0);
    ray = ray1;
}

void tavli::PieceDragger::on_drop(vvr::Drawable* drw)
{
    auto piece = static_cast<Piece*>(drw);
    auto region_drop = static_cast<Region*>(regionPicker->picked());
    if (region_drop && region_drop!=piece->region) {
        piece->region->removePiece(piece);
        region_drop->addPiece(piece);
    } else piece->region->arrangePieces(0);
    piece->colour = colour;
    regionPicker->do_drop(ray, 0);
}

/*---[tavli::RegionHighlighter]---------------------------------------------------------*/
bool tavli::RegionHighlighter::on_pick(vvr::Drawable* drw, Ray)
{
    assert(typeid(Region)==typeid(*drw));
    auto reg = static_cast<Region*>(drw);
    colour = reg->colour;
    reg->colour.mul(1.50);
    reg->setColourPerVertex(reg->colour, reg->colour, reg->colour);
    return true;
}

void tavli::RegionHighlighter::on_drop(vvr::Drawable* drw)
{
    auto reg = static_cast<Region*>(drw);
    reg->colour = colour;
    reg->setColourPerVertex(colour, colour, colour);
}

/*---[main]-----------------------------------------------------------------------------*/
namespace tavli {
    std::vector<vvr::Colour> GetDefaultColours()
    {
        std::vector<vvr::Colour> colours = {
            0x443322,  ///> Board
            0x242622,  ///> Regions
            0x550000,  ///> Team 1
            0xBBBBBB   ///> Team 2
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
        assert(argc>0);
        for(size_t coli=0; coli < std::min(colours.size(),(size_t)(argc-1)); coli++) {
            colours[coli] = vvr::Colour(argv[1+coli]);
        }
#endif

        /* Print colours */
        for (auto c : colours) {
            printf("colours[coli++] = vvr::Colour(\"%02X%02X%02X\");\n", c.r, c.g, c.b);
        }
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
