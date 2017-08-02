#ifndef TAVLI_HEADER_240717
#define TAVLI_HEADER_240717

#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vector>

using math::vec;

namespace tavli
{

    struct Piece : public vvr::Circle2D
    {
        void draw() const override;
        void drop();
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
        void resize(const float width, const float height);
        std::vector<Piece*> pieces;
        std::vector<Region*> regions;
        std::vector<vvr::LineSeg2D*> bounds;
        std::vector<vvr::Triangle2D*> wood;
        vvr::Canvas canvas;
        float w;
        float h;
        float d;
        float r;
    };


    struct PieceDragger2D
    {
        bool grab(vvr::Drawable* drw)
        {
            if (auto sh = dynamic_cast<tavli::Piece*>(drw))
            {
                col_org = sh->colour;
                sh->colour.lighther();
                sh->colour.lighther();
                vvr_msg("Grabbed");
                return true;
            }

            vvr_msg("Grab rejected");
            return false;
        }

        void drag(vvr::Drawable* drw, int x, int y)
        {
            if (auto c = dynamic_cast<tavli::Piece*>(drw)) {
                c->Move(C2DPoint(x, y));
            }
        }

        void drop(vvr::Drawable* drw)
        {
            if (auto piece = dynamic_cast<tavli::Piece*>(drw)) {
                piece->colour = col_org;
                piece->drop();
            }
            vvr_msg("Dropped");
        }

    private:
        vvr::Colour col_org;
    };

}

namespace vvr
{

    template <typename Dragger2D>
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

}

class TavliScene : public vvr::Scene
{
public:

    TavliScene();
    ~TavliScene();
    void draw() override;
    void resize() override;
    void reset() override;

    const char* getName() const {
        return "Tavli Game";
    }

    void keyEvent(unsigned char key, bool up, int modif);

    void mousePressed(int x, int y, int modif) override {
        mPicker->mousePressed(x, y, modif);
    }
    void mouseMoved(int x, int y, int modif) override {
        mPicker->mouseMoved(x, y, modif);
    }
    void mouseReleased(int x, int y, int modif) override {
        mPicker->mouseReleased(x, y, modif);
    }

    typedef vvr::MousePicker2D<tavli::PieceDragger2D> PiecePicker;

private:
    vvr::Axes *mAxes;
    tavli::Board *mBoard;
    PiecePicker *mPicker;
};

#endif
