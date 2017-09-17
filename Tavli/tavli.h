#ifndef TAVLI_HEADER_240717
#define TAVLI_HEADER_240717

#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vvr/picking.h>
#include <vvr/mesh.h>
#include <vector>

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
        void on_drag(Drawable* drw, Ray ray0, Ray ray1) {};
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
        Piece(Colour col);
        void draw() const override;
        real pickdist(const Ray& ray) const override;
        void pick();
        void drop();

        /* Data */
        Region* region;
    };

    struct Region : public Triangle3D
    {
        Region(Board* board, int reg, Colour colour);
        real pickdist(const Ray& ray) const override;
        void addPiece(Piece *piece);
        void removePiece(Piece *piece);
        void resize(float diam, float boardheight);
        void arrangePieces(size_t index_from=0);
        
        /* Data [Logic] */
        Board* board;
        std::vector<Piece*> pieces;

        /* Data [Drawing] */
        int id, rows;
        vec base, top, dir;
        float piecediam, boardheight;
    };

    struct Board : public Drawable
    {
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
        
        RegionHlter::Ptr        region_hlter;
        RegionPicker::Ptr       region_picker;
        PieceDragger::Ptr       piece_dragger;
        PiecePicker::Ptr        piece_picker;

    private:
        std::vector<Colour>     _colours;
        std::vector<Piece*>     _pieces;
        std::vector<Region*>    _regions;
        Mesh::Ptr               _3DBoard;
        Canvas                  _pieceCanvas;
        Canvas                  _regionCanvas;
        float                   _width;
        float                   _height;
    };
}

#endif
