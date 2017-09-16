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

    struct RegionChooser
    {
        bool grab(Drawable* drw);
        void drag(Drawable* drw, Ray ray0, Ray ray1);
        void drop(Drawable* drw);

    private:
        Colour colour;
        bool visible;
    };

    struct PieceDragger
    {
        bool grab(Drawable* drw);
        void drag(Drawable* drw, Ray ray0, Ray ray1);
        void drop(Drawable* drw);

        PieceDragger(Canvas &canvas, RegionChooser *rc) 
            : regionPicker(canvas, rc) { }

    private:
        Ray ray;
        Colour colour;
        MousePicker3D<RegionChooser> regionPicker;
    };

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

        typedef MousePicker3D<PieceDragger> PiecePicker;
        typedef MousePicker3D<RegionChooser> RegionPicker;

        /* Data [Logic] */
        float width, height;
        std::vector<Piece*> pieces;
        std::vector<Region*> regions;
        PiecePicker *piecePicker;

        /* Data [Drawing] */
        std::vector<Colour> colours;
        Canvas pieceCanvas;
        Canvas regionCanvas;
        Mesh::Ptr m3DBoard;
    };
}

#endif
