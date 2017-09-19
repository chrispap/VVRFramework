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

    template <class DrawableT, class ContextT=void>
    struct Dragger2D
    {
        bool on_pick(DrawableT* d)
        {
            std::cerr << "Dragger2D<" << typestr(*d) << "> missing." << std::endl;
            return false;
        }

        void on_drag(int dx, int dy) { }

        void on_drop() { }
    };

    template <class ContextT>
    struct Dragger2D<Point3D, ContextT>
    {
        bool on_pick(Point3D* pt)
        {
            _pt = pt;
            _colvirg = pt->colour;
            pt->colour = magenta;
            return true;
        }

        void on_drag(int dx, int dy)
        {
            (*_pt) += vec(dx, dy, 0);
        }

        void on_drop()
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
        bool on_pick(LineSeg3D* ln)
        {
            _picked = ln;
            _colvirg = ln->colour;
            _picked->colour = magenta;
            return true;
        }

        void on_drag(int dx, int dy)
        {
            _picked->Translate(vec(dx, dy, 0));
        }

        void on_drop()
        {
            _picked->colour = _colvirg;
        }

    private:
        LineSeg3D* _picked;
        Colour _colvirg;
    };

    template <class ContextT>
    struct Dragger2D<Triangle3D, ContextT>
    {
        bool on_pick(Triangle3D* tri)
        {
            _picked = tri;
            _colvirg = _picked->colour;
            _picked->colour = magenta;
            _picked->setColourPerVertex(_picked->colour, _picked->colour, _picked->colour);
            return true;
        }

        void on_drag(int dx, int dy)
        {
            _picked->Translate(vec(dx, dy, 0));
        }

        void on_drop()
        {
            _picked->colour = _colvirg;
            _picked->setColourPerVertex(_picked->colour, _picked->colour, _picked->colour);
        }

    private:
        Triangle3D* _picked;
        Colour _colvirg;
    };

    template <class ContextT>
    struct Dragger2D<Triangle2D, ContextT>
    {
        bool on_pick(Triangle2D* tri)
        {
            _picked = tri;
            return true;
        }

        void on_drag(int dx, int dy)
        {
            _picked->x1 += dx;
            _picked->x2 += dx;
            _picked->x3 += dx;
            _picked->y1 += dy;
            _picked->y2 += dy;
            _picked->y3 += dy;
        }

        void on_drop()
        {

        }

    private:
        Triangle2D *_picked;
    };

    template <class ContextT>
    struct Dragger2D<Circle2D, ContextT>
    {
        bool on_pick(Circle2D* cir)
        {
            _picked = cir;
            return true;
        }

        void on_drag(int dx, int dy)
        {
            _picked->Move(C2DVector(dx,dy));
        }

        void on_drop()
        {

        }

    private:
        Circle2D *_picked;
    };

    template <class WholeT, class BlockT, size_t N, class ContextT>
    struct Dragger2D<Composite<WholeT, BlockT, N>, ContextT>
    {
        typedef Composite<WholeT, BlockT, N> CompositeT;

        bool on_pick(CompositeT* drw)
        {
            _picked = drw;
            return _grabber_whole.on_pick(&drw->whole);
        }

        void on_drag(int dx, int dy)
        {
            for (auto component : _picked->blocks) {
                _grabber_block.on_pick(component);
                _grabber_block.on_drag(dx, dy);
                _grabber_block.on_drop();
            }
        }

        void on_drop()
        {
            _grabber_whole.on_drop();
        }

private:
        Dragger2D<WholeT>   _grabber_whole;
        Dragger2D<BlockT>   _grabber_block;
        CompositeT*         _picked;
        Colour              _colvirg;
    };

    /*---[MousePicker: 2D]--------------------------------------------------------------*/
    template <class DrawableT, class ContextT=void>
    struct MousePicker2D
    {
        vvr_decl_shared_ptr(MousePicker2D)

        typedef Dragger2D<DrawableT, ContextT> DraggerT;

        bool pick(int x, int y, int modif)
        {
            if (_picked) {
                drop(x, y, modif);
                _picked = nullptr;
            }

            const bool dupl = Scene::altDown(modif);
            _mousepos = { x,y };

            if ((_picked = query(_mousepos)))
            {
                DrawableT* ddr = nullptr;
                if (dupl) {
                    _picked = ddr = new DrawableT(*_picked);
                    ddr->addToCanvas(_canvas);
                }
                if (_dragger.on_pick(_picked)) return true;
                else delete ddr;
            }

            _picked = nullptr;
            return false;
        }

        void drag(int x, int y, int modif)
        {
            if (!_picked) return;
            int dx = x - _mousepos.x;
            int dy = y - _mousepos.y;
            _dragger.on_drag(dx, dy);
            _mousepos = { x,y };
        }

        void drop(int x, int y, int modif)
        {
            if (!_picked) return;
            int dx = x - _mousepos.x;
            int dy = y - _mousepos.y;
            _dragger.on_drop();
            _mousepos = { x,y };
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
            , _mousepos({ 0,0 })
        { }

    private:
        DrawableT*  _picked;
        Canvas&     _canvas;
        DraggerT    _dragger;
        Mousepos    _mousepos;
    };

    /*---[MousePicker: 3D]--------------------------------------------------------------*/
    template <class DraggerT>
    struct MousePicker3D
    {
        vvr_decl_shared_ptr(MousePicker3D)

        void pick(math::Ray ray, int modif)
        {
            if (_picked) {
                drop(ray, modif);
                _picked = nullptr;
            }
            
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

    /*---[MousePicker: 2D-Cascade]------------------------------------------------------*/
    template <class... PickerTs>
    struct CascadePicker2D
    {
        vvr_decl_shared_ptr(CascadePicker2D)

        typedef std::tuple<PickerTs...> picker_tuple_t;

        picker_tuple_t pickers;

        CascadePicker2D(Canvas &canvas)
            : pickers(std::make_tuple(std::forward<PickerTs>(canvas)...))
        { }

        bool pick(int x, int y, int modif)
        {
            apply(pickers, [&](auto &p) {
                p.drop(x, y, modif);
            });

            bool picked = false;
            apply(pickers, [&](auto &p) {
                picked = picked || p.pick(x, y, modif);
            });
            return picked;
        }

        void drag(int x, int y, int modif)
        {
            apply(pickers, [&](auto &p) {
                p.drag(x, y, modif);
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
