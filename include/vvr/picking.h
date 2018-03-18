#ifndef VVR_PICKING_H
#define VVR_PICKING_H

#include "dragging.h"
#include <tuple>

namespace vvr
{
    /*---[MousePicker: 2D]--------------------------------------------------------------*/
    template <class DrawableT, class ContextT = void>
    struct MousePicker2D
    {
        vvr_decl_shared_ptr(MousePicker2D)

        typedef Dragger2D<DrawableT, ContextT> DraggerT;

        bool do_pick(Mousepos mp, int modif, bool allow_dupl=false)
        {
            if (_picked) {
                do_drop();
                _picked = nullptr;
            }

            const bool dupl = allow_dupl && Scene::ctrlDown(modif);

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

        void do_drag(Mousepos mp, int modif)
        {
            if (!_picked) return;
            _dragger.on_drag(mp);
        }

        void do_drop()
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

    /*---[MousePicker: 3D]--------------------------------------------------------------*/
    template <class DraggerT>
    struct MousePicker3D
    {
        vvr_decl_shared_ptr(MousePicker3D)

        void do_pick(math::Ray ray, int modif)
        {
            do_drop(ray, modif);

            if ((_picked = query(ray))) {
                _mouseray = ray;
                if (!_dragger->on_pick(_picked, ray)) {
                    _picked = nullptr;
                }
            }
        }

        void do_drag(math::Ray ray, int modif)
        {
            if (!_picked) return;
            _dragger->on_drag(_picked, _mouseray, ray);
            _mouseray = ray;
        }

        void do_drop(math::Ray ray, int modif)
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

    /*---[MousePicker: 2D-Priority]-----------------------------------------------------*/
    template <class... PickerTs>
    struct PriorityPicker2D
    {
        vvr_decl_shared_ptr(PriorityPicker2D)

        typedef std::tuple<PickerTs...> picker_tuple_t;

        picker_tuple_t pickers;

        bool do_pick(Mousepos mp, int modif, bool allow_dupl=false)
        {
           do_drop();

            _picked = nullptr;
            apply(pickers, [&](auto &p) {
                if (!_picked && p.do_pick(mp, modif, allow_dupl)) {
                    _picked = p.picked();
                }
            });
            return _picked;
        }

        void do_drag(Mousepos mp, int modif)
        {
            apply(pickers, [&](auto &p) {
                p.do_drag(mp, modif);
            });
        }

        void do_drop()
        {
            apply(pickers, [](auto &p) {
                p.do_drop();
            });
            _picked = nullptr;
        }

        Drawable* picked() const { return _picked; }

        template <size_t I>
        auto picked() { return std::get<I>(pickers).picked(); }

        template <class T>
        auto& get() { return std::get<MousePicker2D<T> >(pickers); }

        template <size_t I>
        auto& get() { return std::get<I>(pickers); }

        PriorityPicker2D(Canvas &canvas)
            : pickers(std::make_tuple(std::forward<PickerTs>(canvas)...))
        { }

    private:
        Drawable* _picked = nullptr;
    };
}

#endif
