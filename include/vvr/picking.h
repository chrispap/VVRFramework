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

    /*---[Draggers----------------------------------------------------------------------*/

    template <class D, class ContextT=void>
    struct Dragger2D
    {
        bool grab(D* d)
        {
            std::cerr << "Dragger2D<"
                      << typestr(*d)
                      << "> missing."
                      << std::endl;
            return false;
        }

        void drag(int dx, int dy) {}

        void drop() {}
    };

    template <class ContextT>
    struct Dragger2D<Point3D, ContextT>
    {
        bool grab(Point3D* pt)
        {
            _pt = pt;
            _colvirg = pt->colour;
            pt->colour = magenta;
            return true;
        }

        void drag(int dx, int dy)
        {
            (*_pt) += vec(dx, dy, 0);
        }

        void drop()
        {
            _pt->colour = _colvirg;
        }

    private:
        Point3D* _pt;
        Colour _colvirg;
    };

    template <class ContextT>
    struct Dragger2D<LineSeg3D, ContextT>
    {
        bool grab(LineSeg3D* ln)
        {
            _drw = ln;
            _colvirg = ln->colour;
            _drw->colour = magenta;
            return true;
        }

        void drag(int dx, int dy)
        {
            _drw->Translate(vec(dx, dy, 0));
        }

        void drop()
        {
            _drw->colour = _colvirg;
        }

    private:
        LineSeg3D* _drw;
        Colour _colvirg;
    };

    template <class ContextT>
    struct Dragger2D<Triangle3D, ContextT>
    {
        bool grab(Triangle3D* tri)
        {
            _drw = tri;
            _colvirg = _drw->colour;
            _drw->colour = magenta;
            _drw->setColourPerVertex(_drw->colour, _drw->colour, _drw->colour);
            return true;
        }

        void drag(int dx, int dy)
        {
            _drw->Translate(vec(dx, dy, 0));
        }

        void drop()
        {
            _drw->colour = _colvirg;
            _drw->setColourPerVertex(_drw->colour, _drw->colour, _drw->colour);
        }

    private:
        Triangle3D* _drw;
        Colour _colvirg;
    };

    template <class ContextT>
    struct Dragger2D<Triangle2D, ContextT>
    {
        bool grab(Triangle2D* tri)
        {
            _drw = tri;
            return true;
        }

        void drag(int dx, int dy)
        {
            _drw->x1 += dx;
            _drw->x2 += dx;
            _drw->x3 += dx;
            _drw->y1 += dy;
            _drw->y2 += dy;
            _drw->y3 += dy;
        }

        void drop()
        {

        }

    private:
        Triangle2D *_drw;
    };

    template <class ContextT>
    struct Dragger2D<Circle2D, ContextT>
    {
        bool grab(Circle2D* cir)
        {
            _drw = cir;
            return true;
        }

        void drag(int dx, int dy)
        {
            _drw->Move(C2DVector(dx,dy));
        }

        void drop()
        {

        }

    private:
        Circle2D *_drw;
    };


    template <class WholeT, class BlockT, size_t N, class ContextT>
    struct Dragger2D<Composite<WholeT, BlockT, N>, ContextT>
    {
        typedef Composite<WholeT, BlockT, N> CompositeDrawable;

        bool grab(CompositeDrawable* drw)
        {
            _drw = drw;
            return _grabber_whole.grab(&drw->whole);
        }

        void drag(int dx, int dy)
        {
            for (auto component : _drw->blocks) {
                _grabber_block.grab(component);
                _grabber_block.drag(dx, dy);
                _grabber_block.drop();
            }
        }

        void drop()
        {
            _grabber_whole.drop();
        }

private:
        Dragger2D<WholeT> _grabber_whole;
        Dragger2D<BlockT> _grabber_block;
        CompositeDrawable* _drw;
        Colour _colvirg;
    };

    /*---[MousePicker: 2D]--------------------------------------------------------------*/

    template <class D, class ContextT=void>
    struct MousePicker2D
    {
        vvr_decl_shared_ptr(MousePicker2D)

        typedef Dragger2D<D, ContextT> dragger_t;

        MousePicker2D(Canvas &canvas, dragger_t dragger = dragger_t())
            : _drw(nullptr)
            , _canvas(canvas)
            , _dragger(dragger)
            , _mousepos({0,0})
        {
        }

        dragger_t& getDragger() { return _dragger; }

        D* query(Mousepos mp)
        {
            if (!_canvas.visible) return nullptr;
            D* nearest = nullptr;
            D* d = nullptr;
            real mindist = std::numeric_limits<real>::max();
            for (auto drw : _canvas.getDrawables()) {
                if (!drw->visible) continue;
                if (!(d=dynamic_cast<D*>(drw))) continue;
                real dist = d->pickdist(mp.x, mp.y);
                if (dist >= 0 && dist < mindist) {
                    nearest = d; mindist = dist;
                }
            }
            return nearest;
        }

        bool pick(int x, int y, int modif)
        {
            const bool dupl = Scene::altDown(modif);
            _mousepos = { x,y };
            if ((_drw = query(_mousepos)))
            {
                D* ddr = nullptr;
                if (dupl) {
                    _drw = ddr = new D(*_drw);
                    ddr->addToCanvas(_canvas);
                }
                if (_dragger.grab(_drw)) return true;
                delete ddr;
            }

            _drw = nullptr;
            return false;
        }

        void move(int x, int y, int modif)
        {
            if (!_drw) return;
            int dx = x - _mousepos.x;
            int dy = y - _mousepos.y;
            _dragger.drag(dx, dy);
            _mousepos = { x,y };
        }

        void drop(int x, int y, int modif)
        {
            if (!_drw) return;
            int dx = x - _mousepos.x;
            int dy = y - _mousepos.y;
            _dragger.drag(dx, dy);
            _dragger.drop();
            _mousepos = { x,y };
            _drw = nullptr;
        }

    private:
        D* _drw;
        Canvas &_canvas;
        dragger_t _dragger;
        Mousepos _mousepos;
    };

    /*---[MousePicker: 3D]--------------------------------------------------------------*/

    template <class Dragger3D>
    struct MousePicker3D
    {
        void pick(math::Ray ray, int modif)
        {
            Drawable *dr_nearest = nullptr;
            real dmin = std::numeric_limits<real>::max();

            for (auto drw : _canvas.getDrawables()) {
                real pickdist = drw->pickdist(ray);
                if (pickdist >= 0 && pickdist < dmin) {
                    dr_nearest = drw;
                    dmin = pickdist;
                }
            }

            if (dr_nearest) {
                _mouseray = ray;
                _drw = dr_nearest;
                if (!_dragger->grab(_drw)) {
                    _drw = nullptr;
                }
            }
        }

        void move(math::Ray ray, int modif)
        {
            if (!_drw) return;
            _dragger->drag(_drw, _mouseray, ray);
            _mouseray = ray;
        }

        void drop(math::Ray ray, int modif)
        {
            if (!_drw) return;
            _dragger->drag(_drw, _mouseray, ray);
            _dragger->drop(_drw);
            _mouseray = ray;
            _drw = nullptr;
        }

        Drawable* getDrawable()
        {
            return _drw;
        }

        MousePicker3D(Canvas &canvas, Dragger3D *dragger)
            : _canvas(canvas)
            , _dragger(dragger)
            , _drw(nullptr)
        { }

        ~MousePicker3D()
        {
            delete _dragger;
        }

    private:
        Canvas &_canvas;
        Dragger3D *_dragger;
        math::Ray _mouseray;
        Drawable* _drw;
    };

    /*---[MousePicker: Cascade]---------------------------------------------------------*/

    template <class... PickerTypes>
    struct CascadePicker2D
    {
        vvr_decl_shared_ptr(CascadePicker2D)

        typedef std::tuple<PickerTypes...> picker_tuple_t;

        picker_tuple_t pickers;

        CascadePicker2D(Canvas &canvas)
            : pickers(std::make_tuple(std::forward<PickerTypes>(canvas)...))
        {

        }

        bool pick(int x, int y, int modif)
        {
            bool picked = false;
            apply(pickers, [&](auto &p) {
                picked = picked || p.pick(x, y, modif);
            });
            return picked;
        }

        void move(int x, int y, int modif)
        {
            apply(pickers, [&](auto &p) {
                p.move(x, y, modif);
            });
        }

        void drop(int x, int y, int modif)
        {
            apply(pickers, [&](auto &p) {
                p.drop(x, y, modif);
            });
        }
    };

}

#endif
