#ifndef TAVLI_HEADER_240717
#define TAVLI_HEADER_240717

#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vector>

using math::vec;

namespace vvr {

    template <typename Dragger>
    struct MousePicker2D
    {
        struct Mousepos { int x, y; };

        void mousePressed(int x, int y, int modif)
        {
            Drawable *drw_nearest = nullptr;
            real_t dmin = std::numeric_limits<real_t>::max();

            for (auto drw : canvas.getDrawables())
            {
                real_t pd = drw->pickdist(x, y);

                if (pd >= 0.0f && pd < dmin)
                {
                    drw_nearest = drw;
                    dmin = pd;
                }
            }

            if (drw_nearest)
            {
                mousepos = { x,y };
                drw = drw_nearest;
                dragger.grab(drw);
            }
        }

        void mouseMoved(int x, int y, int modif)
        {
            if (!drw) return;
            dragger.drag(drw, x - mousepos.x, y - mousepos.y);
            mousepos = { x,y };
        }

        void mouseReleased(int x, int y, int modif)
        {
            if (!drw) return;
            dragger.drag(drw, x - mousepos.x, y - mousepos.y);
            dragger.drop(drw);
            mousepos = { x,y };
            drw = nullptr;
        }

        MousePicker2D(Canvas &canvas, Dragger &dragger)
            : drw(nullptr)
            , canvas(canvas)
            , dragger(dragger)
        { }

    private:
        Drawable* drw;
        Canvas &canvas;
        Dragger &dragger;
        Mousepos mousepos;
    };

    struct Dragger2D
    {
        void grab(Drawable* drw)
        {
            vvr_msg("Grabbed");
            if (auto sh = dynamic_cast<Shape*>(drw)) {
                col_org = sh->colour;
                sh->colour.lighther();
                sh->colour.lighther();
            }
        }

        void drag(Drawable* drw, int x, int y)
        {
            if (auto c = dynamic_cast<Circle2D*>(drw)) {
                c->Move(C2DPoint(x, y));
            }
        }

        void drop(Drawable* drw)
        {
            vvr_msg("Dropped");
            if (auto sh = dynamic_cast<Shape*>(drw)) {
                sh->colour = col_org;
            }
        }

    private:
        Colour col_org;
    };

}

namespace tavli
{
    struct Piece : public vvr::Circle2D
    {
        void draw() const override;
    };

    struct Region : public vvr::Drawable
    {
        void draw() const override;
        vvr::Triangle2D tri;
    };

    struct Board : public vvr::Drawable
    {
        Board();
        void draw() const override;
        void resize(float width, float height);
        std::vector<Piece*> pieces;
        std::vector<Region*> regions;
        std::vector<vvr::LineSeg2D*> bounds;
        std::vector<vvr::Triangle2D*> wood;
        vvr::Canvas canvas;
        float w, h;
    };

    class Scene : public vvr::Scene
    {
    public:
        Scene();
        const char* getName() const;
        void draw() override;
        void resize() override;
        void reset() override;
        void keyEvent(unsigned char key, bool up, int modif) override;
        void mousePressed(int x, int y, int modif) override;
        void mouseMoved(int x, int y, int modif) override;
        void mouseReleased(int x, int y, int modif) override;

    private:
        vvr::Axes *mAxes;
        Board *mBoard;
        vvr::Dragger2D mDragger;
        vvr::MousePicker2D<vvr::Dragger2D> *mPicker;
    };
}

#endif
