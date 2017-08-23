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
            Drawable *dr_nearest = nullptr;
            real dmin = std::numeric_limits<real>::max();

            for (auto dr : canvas.getDrawables())
            {
                real pd = dr->pickdist(x, y);

                if (pd >= 0 && pd < dmin)
                {
                    dr_nearest = dr;
                    dmin = pd;
                }
            }

            if (dr_nearest)
            {
                mousepos = { x,y };
                dr = dr_nearest;
                if (!dragger->grab(dr)) {
                    dr = nullptr;
                }
            }
        }

        void mouseMoved(int x, int y, int modif)
        {
            if (!dr) return;
            dragger->drag(dr, x - mousepos.x, y - mousepos.y);
            mousepos = { x,y };
        }

        void mouseReleased(int x, int y, int modif)
        {
            if (!dr) return;
            dragger->drag(dr, x - mousepos.x, y - mousepos.y);
            dragger->drop(dr);
            mousepos = { x,y };
            dr = nullptr;
        }

        MousePicker2D(Canvas &canvas, Dragger2D *dragger)
            : dr(nullptr)
            , canvas(canvas)
            , dragger(dragger)
        { }

        ~MousePicker2D()
        {
            delete dragger;
        }

    private:
        Dragger2D *dragger;
        Drawable* dr;
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
            Drawable *dr_nearest = nullptr;
            real dmin = std::numeric_limits<real>::max();

            for (auto dr : canvas.getDrawables()) {
                real pickdist = dr->pickdist(ray);
                if (pickdist >= 0 && pickdist < dmin) {
                    dr_nearest = dr;
                    dmin = pickdist;
                }
            }

            if (dr_nearest) {
                mouseray = ray;
                dr = dr_nearest;
                if (!dragger->grab(dr)) {
                    dr = nullptr;
                }
            }
        }

        /**
         * Move the picked drawing if any.
         */
        void mouseMoved(math::Ray ray, int modif)
        {
            if (!dr) return;
            dragger->drag(dr, mouseray, ray);
            mouseray = ray;
        }

        /**
         * Drop the picked drawing if any.
         */
        void mouseReleased(math::Ray ray, int modif)
        {
            if (!dr) return;
            dragger->drag(dr, mouseray, ray);
            dragger->drop(dr);
            mouseray = ray;
            dr = nullptr;
        }

        Drawable* getDrawable()
        {
            return dr;
        }

        MousePicker3D(Canvas &canvas, Dragger3D *dragger)
            : canvas(canvas)
            , dragger(dragger)
            , dr(nullptr)
        { }

        ~MousePicker3D()
        {
            delete dragger;
        }

    private:
        Canvas &canvas;
        Dragger3D *dragger;
        math::Ray mouseray;
        Drawable* dr;
    };
}

#endif
