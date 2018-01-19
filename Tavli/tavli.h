#ifndef TAVLI_HEADER_240717
#define TAVLI_HEADER_240717

#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vvr/picking.h>
#include <vvr/mesh.h>
#include <vector>

namespace vvr
{
    /*---[MousePicker: 3D]--------------------------------------------------------------*/
    template <class DraggerT>
    struct MousePicker3D
    {
        vvr_decl_shared_ptr(MousePicker3D)

        void pick(math::Ray ray, int modif)
        {
            drop(ray, modif);

            if ((_picked = query(ray))) {
                _mouseray = ray;
                if (!_dragger->on_pick(_picked)) {
                    _picked = nullptr;
                }
            }
        }

        void drag(math::Ray ray, int modif)
        {
            if (!_picked) return;
            _dragger->on_drag(_picked, _mouseray, ray);
            _mouseray = ray;
        }

        void drop(math::Ray ray, int modif)
        {
            if (!_picked) return;
            _dragger->on_drop(_picked);
            _mouseray = ray;
            _picked = nullptr;
        }

        Drawable* query(const math::Ray& ray)
        {
            if (!_canvas.visible) return nullptr;
            Drawable *nearest = nullptr;
            real dmin = std::numeric_limits<real>::max();

            for (auto drw : _canvas.getDrawables()) {
                real pickdist = drw->pickdist(ray);
                if (pickdist >= 0 && pickdist < dmin) {
                    nearest = drw;
                    dmin = pickdist;
                }
            }

            return nearest;
        }

        Drawable* picked() { return _picked; }

        DraggerT* dragger() { return _dragger; }

        MousePicker3D(Canvas &canvas, DraggerT *dragger)
            : _canvas(canvas)
            , _dragger(dragger)
            , _picked(nullptr)
        { }

    private:
        Canvas&     _canvas;
        DraggerT*   _dragger;
        Drawable*   _picked;
        math::Ray   _mouseray;
    };
}

namespace tavli
{
    using namespace vvr;
    using namespace math;

    struct Piece;
    struct Region;
    struct Board;

    struct RegionHlter
    {
        vvr_decl_shared_ptr(RegionHlter)

        bool on_pick(Drawable* drw);
        void on_drag(Drawable* drw, Ray ray0, Ray ray1) {}
        void on_drop(Drawable* drw);

    private:
        Colour _colour;
    };

    typedef MousePicker3D<RegionHlter> RegionPicker;

    struct PieceDragger
    {
        vvr_decl_shared_ptr(PieceDragger)

        bool on_pick(Drawable* drw);
        void on_drag(Drawable* drw, Ray ray0, Ray ray1);
        void on_drop(Drawable* drw);

        PieceDragger(RegionPicker* rp)
            : _regionPicker{ rp } {}

    private:
        Ray _ray;
        Colour _colour;
        RegionPicker* _regionPicker;
    };

    typedef MousePicker3D<PieceDragger> PiecePicker;

    struct Piece : public Cylinder3D
    {
        Region* region;
        Piece(Colour col);
        void pick();
        void draw() const override;
        real pickdist(const Ray& ray) const override;
    };

    struct Region : public Triangle3D
    {
        /* Data [Logic] */
        Board* board;
        std::vector<Piece*> pieces;

        /* Data [Drawing] */
        float piecediam, boardheight;
        int id, rows;
        vec base, top, dir;

        Region(Board* board, int reg, Colour colour);
        void addPiece(Piece *piece);
        void removePiece(Piece *piece);
        void resize(float diam, float boardheight);
        void arrangePieces(size_t index_from=0);
        real pickdist(const Ray& ray) const override;
    };

    class Board : public Drawable
    {
        std::vector<Colour>     _colours;
        std::vector<Piece*>     _pieces;
        std::vector<Region*>    _regions;
        Mesh::Ptr               _3DBoard;
        Canvas                  _pieceCanvas;
        Canvas                  _regionCanvas;
        float                   _width;
        float                   _height;

    public:
        RegionHlter::Ptr        region_hlter;
        RegionPicker::Ptr       region_picker;
        PieceDragger::Ptr       piece_dragger;
        PiecePicker::Ptr        piece_picker;

        Board(std::vector<Colour> &cols);
        ~Board();
        void load3DModels();
        void createRegions();
        void clearPieces();
        void setupGamePortes();
        void setupGamePlakwto();
        void setupGameFevga();
        void draw() const override;
        void resize(const float width, const float height);
    };
}

#endif
