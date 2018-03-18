#ifndef VVR_DRAGGING_H
#define VVR_DRAGGING_H

#include "drawing.h"

namespace vvr
{
    struct Mousepos
    {
        int x, y;
    };

    struct DraggerBase
    {
        Colour col_hover = vvr::LightSeaGreen;
    protected:
        Colour colvirg;
    };

    /*---[Draggers: 2D]-----------------------------------------------------------------*/
    template <class DrawableT, class ContextT = void>
    struct Dragger2D
    {
        bool on_pick(Mousepos, DrawableT*)
        {
            static_assert(sizeof(DrawableT)==0, "Should provide specialization");
        }

        void on_drag(Mousepos)
        {
            static_assert(sizeof(DrawableT)==0, "Should provide specialization");
        }

        void on_drop()
        {
            static_assert(sizeof(DrawableT)==0, "Should provide specialization");
        }
    };

    template <class ContextT>
    struct Dragger2D<Point3D, ContextT> : DraggerBase
    {
        bool on_pick(Mousepos mp, Point3D* pt)
        {
            picked = pt;
            colvirg = picked->colour;
            picked->colour = col_hover;
            return true;
        }

        void on_drag(Mousepos mp)
        {
            picked->setGeom(vec(mp.x, mp.y, 0));
        }

        void on_drop()
        {
            picked->colour = colvirg;
        }

    private:
        Point3D* picked;
    };

    template <class ContextT>
    struct Dragger2D<LineSeg3D, ContextT> : DraggerBase
    {
        bool on_pick(Mousepos mp, LineSeg3D* ln)
        {
            picked = ln;
            mplast = mp;
            colvirg = picked->colour;
            picked->colour = col_hover;
            return true;
        }

        void on_drag(Mousepos mp)
        {
            picked->Translate(vec(mp.x-mplast.x, mp.y - mplast.y, 0));
            mplast = mp;
        }

        void on_drop()
        {
            picked->colour = colvirg;
        }

    private:
        LineSeg3D* picked;
        Mousepos mplast;
    };

    template <class ContextT>
    struct Dragger2D<Triangle3D, ContextT> : DraggerBase
    {
        bool on_pick(Mousepos mp, Triangle3D* tri)
        {
            picked = tri;
            mplast = mp;
            colvirg = picked->colour;
            picked->colour = col_hover;
            picked->setColourPerVertex(picked->colour, picked->colour, picked->colour);
            return true;
        }

        void on_drag(Mousepos mp)
        {
            picked->Translate(vec(mp.x - mplast.x, mp.y - mplast.y, 0));
            mplast = mp;
        }

        void on_drop()
        {
            picked->colour = colvirg;
            picked->setColourPerVertex(picked->colour, picked->colour, picked->colour);
        }

    private:
        Triangle3D* picked;
        Mousepos mplast;
    };

    template <class ContextT>
    struct Dragger2D<Triangle2D, ContextT> : DraggerBase
    {
        bool on_pick(Mousepos mp, Triangle2D* tri)
        {
            mplast = mp;
            picked = tri;
            colvirg = picked->colour;
            picked->colour = col_hover;
            return true;
        }

        void on_drag(Mousepos mp)
        {
            const int dx = mp.x - mplast.x;
            const int dy = mp.y - mplast.y;
            picked->x1 += dx;
            picked->x2 += dx;
            picked->x3 += dx;
            picked->y1 += dy;
            picked->y2 += dy;
            picked->y3 += dy;
            mplast = mp;
        }

        void on_drop()
        {
            picked->colour = colvirg;
        }

    private:
        Triangle2D *picked;
        Mousepos mplast;
    };

    template <class ContextT>
    struct Dragger2D<Circle2D, ContextT> : DraggerBase
    {
        bool on_pick(Mousepos mp, Circle2D* cir)
        {
            picked = cir;
            colvirg = picked->colour;
            picked->colour = col_hover;
            return true;
        }

        void on_drag(Mousepos mp)
        {
            picked->SetCentre(C2DPoint(mp.x, mp.y));
        }

        void on_drop()
        {
            picked->colour = colvirg;
        }

    private:
        Circle2D *picked;
    };

    template <class WholeT, class BlockT, size_t N, class ContextT>
    struct Dragger2D<Composite<WholeT, BlockT, N>, ContextT> : DraggerBase
    {
        typedef Composite<WholeT, BlockT, N> composite_t;

        bool on_pick(Mousepos mp, composite_t* drw)
        {
            size_t i=0;
            mplast = mp;
            picked = drw;
            if (!dragger_whole.on_pick(mp, &drw->whole)) {
                return false;
            }
            for (auto comp : picked->blocks) {
                dragger_block[i++].on_pick(mp, comp);
            }
            return true;
        }

        void on_drag(Mousepos mp)
        {
            size_t i=0;
            Mousepos mpd;
            for (auto comp : picked->blocks) {
                mpd.x = mp.x - mplast.x + comp->x;
                mpd.y = mp.y - mplast.y + comp->y;
                dragger_block[i++].on_drag(mpd);
            }
            mplast = mp;
        }

        void on_drop()
        {
            size_t i=0;
            for (auto comp : picked->blocks) {
                dragger_block[i++].on_drop();
            }
            dragger_whole.on_drop();
        }

    private:
        Dragger2D<WholeT, ContextT> dragger_whole;
        Dragger2D<BlockT, ContextT> dragger_block[N];
        composite_t* picked;
        Mousepos mplast;
    };
}

#endif
