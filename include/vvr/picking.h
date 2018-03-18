#ifndef VVR_PICKING_H
#define VVR_PICKING_H

#include "dragging.h"
#include <tuple>

namespace vvr
{
    /*---[MousePickers: 2D]-------------------------------------------------------------*/
    template <class DrawableT, class ContextT = void>
    struct MousePicker2D
    {
        vvr_decl_shared_ptr(MousePicker2D)

        typedef Dragger2D<DrawableT, ContextT> dragger_t;

        MousePicker2D(Canvas &canvas) : picked(nullptr), canvas(canvas) {}

        DrawableT* query(Mousepos mp) const
        {
            if (!canvas.visible) return nullptr;
            DrawableT* nearest = nullptr;
            DrawableT* d = nullptr;
            real mindist = std::numeric_limits<real>::max();
            for (auto drw : canvas.getDrawables()) {
                if (!drw->visible) continue;
                if (!(d = dynamic_cast<DrawableT*>(drw))) continue;
                real dist = d->pickdist(mp.x, mp.y);
                if (dist >= 0 && dist < mindist) {
                    nearest = d; mindist = dist;
                }
            }
            return nearest;
        }

        bool do_pick(Mousepos mp, int modif, bool duplicate=false)
        {
            if (picked) {
                do_drop();
                picked = nullptr;
            }

            if ((picked = query(mp)))
            {
                DrawableT* ddr = nullptr;
                if (duplicate) {
                    picked = ddr = new DrawableT(*picked);
                    ddr->collect(canvas);
                }
                if (dragger.on_pick(mp, picked)) return true;
                else delete ddr;
            }

            picked = nullptr;
            return false;
        }

        void do_drag(Mousepos mp, int modif)
        {
            if (!picked) return;
            dragger.on_drag(mp);
        }

        void do_drop()
        {
            if (!picked) return;
            dragger.on_drop();
            picked = nullptr;
        }

        DrawableT* get_picked() { return picked; }

    public:
        dragger_t    dragger;

    private:
        DrawableT*  picked;
        Canvas&     canvas;
    };

    template <class... PickerTs>
    struct PriorityPicker2D
    {
        vvr_decl_shared_ptr(PriorityPicker2D)

        typedef std::tuple<PickerTs...> picker_tuple_t;

        PriorityPicker2D(Canvas &canvas)
            : pickers(std::make_tuple(std::forward<PickerTs>(canvas)...))
        {}

        bool do_pick(Mousepos mp, int modif, bool duplicate=false)
        {
           do_drop();

            picked = nullptr;
            apply(pickers, [&](auto &p) {
                if (!picked && p.do_pick(mp, modif, duplicate)) {
                    picked = p.get_picked();
                }
            });
            return picked;
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
            picked = nullptr;
        }

        Drawable* get_picked() const { return picked; }

        template <class T>
        auto get_picked() { return std::get<MousePicker2D<T> >(pickers).picked(); }

        template <size_t I>
        auto get_picked() { return std::get<I>(pickers).picked(); }

        template <class T>
        auto& get_picker() { return std::get<MousePicker2D<T> >(pickers); }

        template <size_t I>
        auto& get_picker() { return std::get<I>(pickers); }

    private:
        picker_tuple_t pickers;
        Drawable* picked = nullptr;
    };

    /*---[MousePickers: 3D]-------------------------------------------------------------*/
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
}

#endif
