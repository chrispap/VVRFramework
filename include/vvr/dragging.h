#ifndef VVR_DRAGGING_H
#define VVR_DRAGGING_H

#include "drawing.h"
#include "scene.h"
#include "utils.h"
#include "palette.h"

namespace vvr
{
    struct DraggerBase
    {
        Colour col_hover = vvr::LightSeaGreen;
    protected:
        Colour _col_virgin;
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
            _picked = pt;
            _col_virgin = _picked->colour;
            _picked->colour = col_hover;
            return true;
        }

        void on_drag(Mousepos mp)
        {
            _picked->setGeom(vec(mp.x, mp.y, 0));
        }

        void on_drop()
        {
            _picked->colour = _col_virgin;
        }

    private:
        Point3D* _picked;
    };

    template <class ContextT>
    struct Dragger2D<LineSeg3D, ContextT> : DraggerBase
    {
        bool on_pick(Mousepos mp, LineSeg3D* ln)
        {
            _picked = ln;
            _mp = mp;
            _col_virgin = _picked->colour;
            _picked->colour = col_hover;
            return true;
        }

        void on_drag(Mousepos mp)
        {
            _picked->Translate(vec(mp.x-_mp.x, mp.y - _mp.y, 0));
            _mp = mp;
        }

        void on_drop()
        {
            _picked->colour = _col_virgin;
        }

    private:
        LineSeg3D* _picked;
        Mousepos _mp;
    };

    template <class ContextT>
    struct Dragger2D<Triangle3D, ContextT> : DraggerBase
    {
        bool on_pick(Mousepos mp, Triangle3D* tri)
        {
            _picked = tri;
            _mp = mp;
            _col_virgin = _picked->colour;
            _picked->colour = col_hover;
            _picked->setColourPerVertex(_picked->colour, _picked->colour, _picked->colour);
            return true;
        }

        void on_drag(Mousepos mp)
        {
            _picked->Translate(vec(mp.x - _mp.x, mp.y - _mp.y, 0));
            _mp = mp;
        }

        void on_drop()
        {
            _picked->colour = _col_virgin;
            _picked->setColourPerVertex(_picked->colour, _picked->colour, _picked->colour);
        }

    private:
        Triangle3D* _picked;
        Mousepos _mp;
    };

    template <class ContextT>
    struct Dragger2D<Triangle2D, ContextT> : DraggerBase
    {
        bool on_pick(Mousepos mp, Triangle2D* tri)
        {
            _mp = mp;
            _picked = tri;
            _col_virgin = _picked->colour;
            _picked->colour = col_hover;
            return true;
        }

        void on_drag(Mousepos mp)
        {
            const int dx = mp.x - _mp.x;
            const int dy = mp.y - _mp.y;
            _picked->x1 += dx;
            _picked->x2 += dx;
            _picked->x3 += dx;
            _picked->y1 += dy;
            _picked->y2 += dy;
            _picked->y3 += dy;
            _mp = mp;
        }

        void on_drop()
        {
            _picked->colour = _col_virgin;
        }

    private:
        Triangle2D *_picked;
        Mousepos _mp;
    };

    template <class ContextT>
    struct Dragger2D<Circle2D, ContextT> : DraggerBase
    {
        bool on_pick(Mousepos mp, Circle2D* cir)
        {
            _picked = cir;
            _col_virgin = _picked->colour;
            _picked->colour = col_hover;
            return true;
        }

        void on_drag(Mousepos mp)
        {
            _picked->SetCentre(C2DPoint(mp.x, mp.y));
        }

        void on_drop()
        {
            _picked->colour = _col_virgin;
        }

    private:
        Circle2D *_picked;
    };

    template <class WholeT, class BlockT, size_t N, class ContextT>
    struct Dragger2D<Composite<WholeT, BlockT, N>, ContextT> : DraggerBase
    {
        typedef Composite<WholeT, BlockT, N> CompositeT;

        bool on_pick(Mousepos mp, CompositeT* drw)
        {
            _mp = mp;
            _picked = drw;
            return _grabber_whole.on_pick(mp, &drw->whole);
        }

        void on_drag(Mousepos mp)
        {
            for (auto comp : _picked->blocks) {
                Mousepos dmp;
                dmp.x = mp.x - _mp.x + comp->x;
                dmp.y = mp.y - _mp.y + comp->y;
                _grabber_block.on_pick(dmp, comp);
                _grabber_block.on_drag(dmp);
                _grabber_block.on_drop();
            }
            _mp = mp;
        }

        void on_drop()
        {
            _grabber_whole.on_drop();
        }

    private:
        Dragger2D<WholeT>   _grabber_whole;
        Dragger2D<BlockT>   _grabber_block;
        CompositeT*         _picked;
        Mousepos            _mp;
    };
}

#endif
