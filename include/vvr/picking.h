#ifndef VVR_PICKING_H
#define VVR_PICKING_H

#include "drawing.h"

namespace vvr
{

    template <class Dragger2D>
    struct MousePicker2D
    {
        struct Mousepos { int x, y; };

        void mousePressed(int x, int y, int modif)
        {
            Drawable *drw_nearest = nullptr;
            real dmin = std::numeric_limits<real>::max();

            for (auto drw : canvas.getDrawables())
            {
                real pd = drw->pickdist(x, y);

                if (pd >= 0 && pd < dmin)
                {
                    drw_nearest = drw;
                    dmin = pd;
                }
            }

            if (drw_nearest)
            {
                mousepos = { x,y };
                drw = drw_nearest;
                if (!dragger->grab(drw)) {
                    drw = nullptr;
                }
            }
        }

        void mouseMoved(int x, int y, int modif)
        {
            if (!drw) return;
            dragger->drag(drw, x - mousepos.x, y - mousepos.y);
            mousepos = { x,y };
        }

        void mouseReleased(int x, int y, int modif)
        {
            if (!drw) return;
            dragger->drag(drw, x - mousepos.x, y - mousepos.y);
            dragger->drop(drw);
            mousepos = { x,y };
            drw = nullptr;
        }

        MousePicker2D(Canvas &canvas, Dragger2D *dragger)
            : drw(nullptr)
            , canvas(canvas)
            , dragger(dragger)
        { }

        ~MousePicker2D()
        {
            delete dragger;
        }

    private:
        Dragger2D *dragger;
        Drawable* drw;
        Canvas &canvas;
        Mousepos mousepos;
    };

    /**
     * @class Dragger3D
     * @method bool Dragger3D::grab(Drawable*);
     * @method void Dragger3D::drag(Drawable*, Ray, Ray);
     * @method void Dragger3D::drop(Drawable*);
     */
    template <class Dragger3D>
    struct MousePicker3D
    {
        /**
         * Pick the closest drawable.
         */
        void mousePressed(math::Ray ray, int modif)
        {
            Drawable *drw_nearest = nullptr;
            real dmin = std::numeric_limits<real>::max();

            for (auto drw : canvas.getDrawables()) {
                real pickdist = drw->pickdist(ray);
                if (pickdist >= 0 && pickdist < dmin) {
                    drw_nearest = drw;
                    dmin = pickdist;
                }
            }

            if (drw_nearest) {
                mouseray = ray;
                drw = drw_nearest;
                if (!dragger->grab(drw)) {
                    drw = nullptr;
                }
            }
        }

        /**
         * Move the picked drawing if any.
         */
        void mouseMoved(math::Ray ray, int modif)
        {
            if (!drw) return;
            dragger->drag(drw, mouseray, ray);
            mouseray = ray;
        }

        /**
         * Drop the picked drawing if any.
         */
        void mouseReleased(math::Ray ray, int modif)
        {
            if (!drw) return;
            dragger->drag(drw, mouseray, ray);
            dragger->drop(drw);
            mouseray = ray;
            drw = nullptr;
        }

        MousePicker3D(Canvas &canvas, Dragger3D *dragger)
            : canvas(canvas)
            , dragger(dragger)
            , drw(nullptr)
        { }

        ~MousePicker3D()
        {
            delete dragger;
        }

    private:
        Canvas &canvas;
        Dragger3D *dragger;
        Drawable* drw;
        math::Ray mouseray;
    };

}

#endif
