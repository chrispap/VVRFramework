#ifndef VVR_PICKING_H
#define VVR_PICKING_H

#include "drawing.h"
#include "scene.h"
#include "utils.h"
#include <tuple>

namespace vvr
{
    struct Mousepos
    {
        int x, y;
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
    struct Dragger2D<Point3D, ContextT>
    {
        bool on_pick(Mousepos mp, Point3D* pt)
        {
            _picked = pt;
            _colvirg = vvr::magenta;
            std::swap(_colvirg, _picked->colour);
            return true;
        }

        void on_drag(Mousepos mp)
        {
            _picked->setGeom(vec(mp.x, mp.y, 0));
        }

        void on_drop()
        {
            _picked->colour = _colvirg;
        }

    private:
        Point3D* _picked;
        Colour _colvirg;
    };

    template <class ContextT>
    struct Dragger2D<LineSeg3D, ContextT>
    {
        bool on_pick(Mousepos mp, LineSeg3D* ln)
        {
            _picked = ln;
            _mp = mp;
            _colvirg = vvr::magenta;
            std::swap(_colvirg, _picked->colour);
            return true;
        }

        void on_drag(Mousepos mp)
        {
            _picked->Translate(vec(mp.x-_mp.x, mp.y - _mp.y, 0));
            _mp = mp;
        }

        void on_drop()
        {
            _picked->colour = _colvirg;
        }

    private:
        LineSeg3D* _picked;
        Mousepos _mp;
        Colour _colvirg;
    };

    template <class ContextT>
    struct Dragger2D<Triangle3D, ContextT>
    {
        bool on_pick(Mousepos mp, Triangle3D* tri)
        {
            _picked = tri;
            _mp = mp;
            _colvirg = vvr::magenta;
            std::swap(_colvirg, _picked->colour);
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
            _picked->colour = _colvirg;
            _picked->setColourPerVertex(_picked->colour, _picked->colour, _picked->colour);
        }

    private:
        Triangle3D* _picked;
        Mousepos _mp;
        Colour _colvirg;
    };

    template <class ContextT>
    struct Dragger2D<Triangle2D, ContextT>
    {
        bool on_pick(Mousepos mp, Triangle2D* tri)
        {
            _mp = mp;
            _picked = tri;
            _colvirg = vvr::magenta;
            std::swap(_colvirg, _picked->colour);
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
            _picked->colour = _colvirg;
        }

    private:
        Triangle2D *_picked;
        Mousepos _mp;
        Colour _colvirg;
    };

    template <class ContextT>
    struct Dragger2D<Circle2D, ContextT>
    {
        bool on_pick(Mousepos mp, Circle2D* cir)
        {
            _picked = cir;
            _colvirg = vvr::magenta;
            std::swap(_colvirg, _picked->colour);
            return true;
        }

        void on_drag(Mousepos mp)
        {
            _picked->SetCentre(C2DPoint(mp.x, mp.y));
        }

        void on_drop()
        {
            std::swap(_colvirg, _picked->colour);
        }

    private:
        Circle2D *_picked;
        Colour _colvirg;
    };

    template <class WholeT, class BlockT, size_t N, class ContextT>
    struct Dragger2D<Composite<WholeT, BlockT, N>, ContextT>
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
                Mousepos dmp {
                    mp.x - _mp.x + (int)comp->x,
                    mp.y - _mp.y + (int)comp->y
                };
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
        Colour              _colvirg;
    };

    /*---[MousePicker: 2D]--------------------------------------------------------------*/
    template <class DrawableT, class ContextT = void>
    struct MousePicker2D
    {
        vvr_decl_shared_ptr(MousePicker2D)

        typedef Dragger2D<DrawableT, ContextT> DraggerT;

        bool pick(Mousepos mp, int modif)
        {
            if (_picked) {
                drop();
                _picked = nullptr;
            }

            const bool dupl = Scene::ctrlDown(modif);

            if ((_picked = query(mp)))
            {
                DrawableT* ddr = nullptr;
                if (dupl) {
                    _picked = ddr = new DrawableT(*_picked);
                    ddr->addToCanvas(_canvas);
                }
                if (_dragger.on_pick(mp, _picked)) return true;
                else delete ddr;
            }

            _picked = nullptr;
            return false;
        }

        void drag(Mousepos mp, int modif)
        {
            if (!_picked) return;
            _dragger.on_drag(mp);
        }

        void drop()
        {
            if (!_picked) return;
            _dragger.on_drop();
            _picked = nullptr;
        }

        DrawableT* query(Mousepos mp) const
        {
            if (!_canvas.visible) return nullptr;
            DrawableT* nearest = nullptr;
            DrawableT* d = nullptr;
            real mindist = std::numeric_limits<real>::max();
            for (auto drw : _canvas.getDrawables()) {
                if (!drw->visible) continue;
                if (!(d = dynamic_cast<DrawableT*>(drw))) continue;
                real dist = d->pickdist(mp.x, mp.y);
                if (dist >= 0 && dist < mindist) {
                    nearest = d; mindist = dist;
                }
            }
            return nearest;
        }

        DrawableT* picked() { return _picked; }

        DraggerT& dragger() { return _dragger; }

        MousePicker2D(Canvas &canvas, DraggerT dragger = DraggerT())
            : _picked(nullptr)
            , _canvas(canvas)
            , _dragger(dragger)
        { }

    private:
        DrawableT*  _picked;
        Canvas&     _canvas;
        DraggerT    _dragger;
    };

    /*---[MousePicker: 2D-Cascade]------------------------------------------------------*/
    template <class... PickerTs>
    struct CascadePicker2D
    {
        vvr_decl_shared_ptr(CascadePicker2D)

        typedef std::tuple<PickerTs...> picker_tuple_t;

        picker_tuple_t pickers;

        bool pick(Mousepos mp, int modif)
        {
            apply(pickers, [&](auto &p) {
                p.drop();
            });

            _picked = false;
            apply(pickers, [&](auto &p) {
                _picked = _picked || p.pick(mp, modif);
            });
            return _picked;
        }

        void drag(Mousepos mp, int modif)
        {
            apply(pickers, [&](auto &p) {
                p.drag(mp, modif);
            });
        }

        void drop()
        {
            apply(pickers, [&](auto &p) {
                p.drop();
            });
        }

        bool picked() const { return _picked; }

        CascadePicker2D(Canvas &canvas)
            : pickers(std::make_tuple(std::forward<PickerTs>(canvas)...))
        { }

    private:
        bool _picked;
    };

}

#endif
