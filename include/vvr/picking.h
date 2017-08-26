#ifndef VVR_PICKING_H
#define VVR_PICKING_H

#include "drawing.h"

namespace vvr
{
    struct Mousepos
    {
        int x, y;
    };

    template <class D>
    struct Dragger2D
    {
        bool grab(D* d)
        {
            std::cerr << "vvr::Dragger2D<" << vvr::typestr(*d) << "> missing." << std::endl;
            return false;
        }

        void drag(int dx, int dy) {}

        void drop() {}
    };

    template <>
    struct Dragger2D<Point3D>
    {
        Point3D* pt;
        Colour colour_pregrab;

        bool grab(Point3D* pt)
        {
            vvr_setmemb(pt);
            colour_pregrab = pt->colour;
            pt->colour = vvr::magenta;
            return true;
        }

        void drag(int dx, int dy)
        {
            pt->x += dx;
            pt->y += dy;
        }

        void drop()
        {
            pt->colour = colour_pregrab;
        }
    };

    template <class D>
    struct MousePicker2D
    {
        vvr_decl_shared_ptr(MousePicker2D<D>)

    private:
        D* dr;
        Canvas *canvas;
        Mousepos mousepos;
        Dragger2D<D> dragger;

    public:
        MousePicker2D(Canvas *canvas)
            : dr(nullptr)
            , canvas(canvas)
        { }

        D* query(Mousepos mp)
        {
            D* nearest = nullptr;
            D* d = nullptr;
            real mindist = std::numeric_limits<real>::max();
            for (auto dr : canvas->getDrawables()) {
                if (!(d=dynamic_cast<D*>(dr))) continue;
                real dist = d->pickdist(mp.x, mp.y);
                if (dist >= 0 && dist < mindist) {
                    nearest = d; mindist = dist;
                }
            }
            return nearest;
        }

        void mousePressed(int x, int y, int modif)
        {
            mousepos = { x,y };
            D* nearest = query(mousepos);
            if (!nearest) return;
            dr = ((dragger.grab(nearest)) ? nearest : nullptr);
        }

        void mouseMoved(int x, int y, int modif)
        {
            if (!dr) return;
            int dx = x - mousepos.x;
            int dy = y - mousepos.y;
            dragger.drag(dx, dy);
            mousepos = { x,y };
        }

        void mouseReleased(int x, int y, int modif)
        {
            if (!dr) return;
            int dx = x - mousepos.x;
            int dy = y - mousepos.y;
            dragger.drag(dx, dy);
            dragger.drop();
            mousepos = { x,y };
            dr = nullptr;
        }
    };

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
