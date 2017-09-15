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
        Point3D* pt;
        Colour colvirg;

        bool grab(Point3D* pt)
        {
            vvr_setmemb(pt);
            colvirg = pt->colour;
            pt->colour = magenta;
            return true;
        }

        void drag(int dx, int dy)
        {
            (*pt) += vec(dx, dy, 0);
        }

        void drop()
        {
            pt->colour = colvirg;
        }
    };

    template <class ContextT>
    struct Dragger2D<LineSeg3D, ContextT>
    {
        LineSeg3D* ln;
        Colour colvirg;

        bool grab(LineSeg3D* ln)
        {
            vvr_setmemb(ln);
            colvirg = ln->colour;
            ln->colour = magenta;
            return true;
        }

        void drag(int dx, int dy)
        {
            ln->Translate(vec(dx, dy, 0));
        }

        void drop()
        {
            ln->colour = colvirg;
        }
    };

    template <class ContextT>
    struct Dragger2D<Triangle3D, ContextT>
    {
        Triangle3D* tri;
        Colour colvirg;

        bool grab(Triangle3D* tri)
        {
            vvr_setmemb(tri);
            colvirg = tri->colour;
            tri->colour = magenta;
            tri->setColourPerVertex(tri->colour, tri->colour, tri->colour);
            return true;
        }

        void drag(int dx, int dy)
        {
            tri->Translate(vec(dx, dy, 0));
        }

        void drop()
        {
            tri->colour = colvirg;
            tri->setColourPerVertex(tri->colour, tri->colour, tri->colour);
        }
    };

    template <class ContextT>
    struct Dragger2D<Triangle2D, ContextT>
    {
        bool grab(Triangle2D* tri)
        {
            vvr_setmemb(tri);
            return true;
        }

        void drag(int dx, int dy)
        {
            tri->x1 += dx;
            tri->x2 += dx;
            tri->x3 += dx;
            tri->y1 += dy;
            tri->y2 += dy;
            tri->y3 += dy;
        }

        void drop()
        {

        }

    private:
        Triangle2D *tri;
    };

    template <class ComponentT, size_t N, class CompositeT, class ContextT>
    struct Dragger2D<Composite<ComponentT, N, CompositeT>, ContextT>
    {
        typedef Composite<ComponentT, N, CompositeT> ConcreteComposite;
        Dragger2D<CompositeT> _grabber_composite;
        Dragger2D<ComponentT> _grabber_component;
        ConcreteComposite* tri;
        Colour colvirg;

        bool grab(ConcreteComposite* tri)
        {
            vvr_setmemb(tri);
            return _grabber_composite.grab(&tri->composite);
        }

        void drag(int dx, int dy)
        {
            for (auto component : tri->components) {
                _grabber_component.grab(component);
                _grabber_component.drag(dx, dy);
                _grabber_component.drop();
            }
        }

        void drop()
        {
            _grabber_composite.drop();
        }
    };

    /*---[MousePicker: 2D]--------------------------------------------------------------*/

    template <class D, class ContextT=void>
    struct MousePicker2D
    {
        vvr_decl_shared_ptr(MousePicker2D)

        typedef Dragger2D<D, ContextT> dragger_t;

    private:
        D* dr;
        Canvas &canvas;
        dragger_t dragger;
        Mousepos mousepos;

    public:
        MousePicker2D(Canvas &canvas, dragger_t dragger = dragger_t())
            : dr(nullptr)
            , canvas(canvas)
            , dragger(dragger)
            , mousepos({0,0})
        {
        }

        dragger_t& getDragger() { return dragger; }

        D* query(Mousepos mp)
        {
            if (!canvas.visible) return nullptr;
            D* nearest = nullptr;
            D* d = nullptr;
            real mindist = std::numeric_limits<real>::max();
            for (auto dr : canvas.getDrawables()) {
                if (!dr->visible) continue;
                if (!(d=dynamic_cast<D*>(dr))) continue;
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
            mousepos = { x,y };
            if (dr = query(mousepos))
            {
                D* ddr = nullptr;
                if (dupl) {
                    dr = ddr = new D(*dr);
                    canvas.add(ddr);
                }
                if (dragger.grab(dr)) return true;
                delete ddr;
            }

            dr = nullptr;
            return false;
        }

        void move(int x, int y, int modif)
        {
            if (!dr) return;
            int dx = x - mousepos.x;
            int dy = y - mousepos.y;
            dragger.drag(dx, dy);
            mousepos = { x,y };
        }

        void drop(int x, int y, int modif)
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

    /*---[MousePicker: 3D]--------------------------------------------------------------*/

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

    /*---[MousePicker: Cascade]---------------------------------------------------------*/

    template <class... PickerTypes>
    struct CascadePicker2D
    {
        vvr_decl_shared_ptr(CascadePicker2D)

        typedef std::tuple<PickerTypes...> picker_tuple_t;

        CascadePicker2D(Canvas &canvas)
            : pickers((sizeof(PickerTypes), canvas)...)
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

        picker_tuple_t pickers;
    };

}

#endif
