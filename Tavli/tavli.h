#ifndef TAVLI_HEADER_240717
#define TAVLI_HEADER_240717

#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vvr/picking.h>
#include <vvr/mesh.h>
#include <vector>

namespace tavli
{
    using math::vec;
    using math::Ray;
    using vvr::real;
    using vvr::Colour;

    class Piece;
    class Region;
    class Board;

    struct Piece : public vvr::Cylinder3D
    {
        Piece(Colour col);
        void draw() const override;
        real pickdist(const Ray& ray) const override;
        void pick();
        void drop();

        /* Data */
        Region* region;
    };

    struct Region : public vvr::Triangle3D
    {
        Region(Board* board, int reg, Colour colour);
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

    struct Board : public vvr::Drawable
    {
        Board(std::vector<Colour> &cols);
        void clearPieces();
        void setupGamePortes();
        void setupGamePlakwto();
        void setupGameFevga();
        void draw() const override;
        void resize(const float width, const float height);

        
        /* Data [Logic] */
        float width, height;
        std::vector<Piece*> pieces;
        std::vector<Region*> regions;

        /* Data [Drawing] */
        std::vector<Colour> colours;
        vvr::Canvas canvas_pieces;
        vvr::Canvas canvas_regions;
        vvr::Mesh::Ptr mBoard3D;

        friend class Piece;
    };

    struct PieceDragger3D
    {
        bool grab(vvr::Drawable* drw);

        void drag(vvr::Drawable* drw, Ray ray0, Ray ray1);

        void drop(vvr::Drawable* drw);

    private:
        Colour col_org;
    };

    typedef vvr::MousePicker2D<PieceDragger3D> PiecePicker2D;
    typedef vvr::MousePicker3D<PieceDragger3D> PiecePicker;
}

#endif
