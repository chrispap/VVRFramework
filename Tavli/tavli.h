#ifndef TAVLI_HEADER_240717
#define TAVLI_HEADER_240717

#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vector>

using math::vec;

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

    template <typename Dragger3D>
    struct MousePicker3D
    {
        void mousePressed(math::Ray ray, int modif)
        {
            Drawable *drw_nearest = nullptr;
            real_t dmin = std::numeric_limits<real_t>::max();

            for (auto drw : canvas.getDrawables()) {
                real_t pickdist = drw->pickdist(ray);
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

        void mouseMoved(math::Ray ray, int modif)
        {
            if (!drw) return;
            dragger->drag(drw, mouseray, ray);
            mouseray = ray;
        }

        void mouseReleased(math::Ray ray, int modif)
        {
            if (!drw) return;
            dragger->drag(drw, mouseray, ray);
            dragger->drop(drw);
            mouseray = ray;
            drw = nullptr;
        }

        MousePicker3D(Canvas &canvas, Dragger3D *dragger)
            : drw(nullptr)
            , canvas(canvas)
            , dragger(dragger)
        { }

        ~MousePicker3D()
        {
            delete dragger;
        }

    private:
        Dragger3D *dragger;
        Drawable* drw;
        Canvas &canvas;
        math::Ray mouseray;
    };
}

namespace tavli
{
    class Piece;
    class Region;
    class Board;

    struct Piece : public vvr::Cylinder3D
    {
        Piece(Board* board, vvr::Colour col);
        void draw() const override;
        vvr::real_t pickdist(const math::Ray& ray) const override
        {
            math::Circle c(diskTop());
            float d;
            bool intersectsPlane = ray.Intersects(c.ContainingPlane(), &d);
            if (!intersectsPlane) return -1;
            float rd = ray.GetPoint(d).Distance(c.pos);
            if (rd > radius) return -1;
            else return rd;
        }
        void drop();
        Region* region;
        Board* board;
    };

    struct Region : public vvr::Triangle3D
    {
        Region(int regcol, vvr::Colour colour);
        void addPiece(Piece *piece);
        void removePiece(Piece *piece);
        void resize(float diam, float boardheight);
        void arrangePieces();
        std::vector<Piece*> pieces;
        vec base;
        vec top;
        vec updir;
        float piecediam;
        float boardheight;
        int regcol;
    };

    struct Board : public vvr::Drawable
    {
        Board(vvr::Colour colt, vvr::Colour col1, vvr::Colour col2);
        void draw() const override;
        void resize(const float width, const float height);
        vvr::Canvas& getCanvas() { return canvas; }
        std::vector<vvr::LineSeg2D*> bounds;
        std::vector<vvr::Triangle2D*> wood;
        std::vector<Region*> regions;
        vvr::Canvas canvas;
        float width;
        float height;

        friend class Piece;
    };

    struct PieceDragger3D
    {
        bool grab(vvr::Drawable* drw)
        {
            if (auto piece = dynamic_cast<tavli::Piece*>(drw))
            {
                vvr_msg("Grabbed piece");
                col_org = piece->colour;
                base_z_org = piece->basecenter.z;
                piece->basecenter.z += piece->height * 2;
                piece->colour.lighther();
                piece->colour.lighther();
                return true;
            } else return false;
        }

        void drag(vvr::Drawable* drw, math::Ray ray0, math::Ray ray1)
        {
            if (auto piece = dynamic_cast<tavli::Piece*>(drw)) {
                float d0, d1;
                math::Plane boardplane(piece->diskBase().ContainingPlane());
                boardplane.Intersects(ray0, &d0);
                boardplane.Intersects(ray1, &d1);
                vec dv(ray1.GetPoint(d1) - ray0.GetPoint(d0));
                piece->basecenter += dv;
            }
        }

        void drop(vvr::Drawable* drw)
        {
            if (auto piece = dynamic_cast<tavli::Piece*>(drw)) {
                vvr_msg("Dropped piece");
                piece->colour = col_org;
                piece->basecenter.z = base_z_org;
                piece->drop();
            }
        }

    private:
        float base_z_org;
        vvr::Colour col_org;
    };

    typedef vvr::MousePicker2D<PieceDragger3D> PiecePicker2D;
    typedef vvr::MousePicker3D<PieceDragger3D> PiecePicker;
}

class TavliScene : public vvr::Scene
{
public:
    TavliScene(vvr::Colour colt, vvr::Colour col1, vvr::Colour col2);
    ~TavliScene();

    const char* getName() const {
        return "Tavli Game";
    }

    void draw() override;
    void resize() override;
    void reset() override;

    void keyEvent(unsigned char key, bool up, int modif);

    void mousePressed(int x, int y, int modif) override {
        if (ctrlDown(modif)) vvr::Scene::mousePressed(x, y, modif);
        mPicker->mousePressed(unproject(x, y), modif);
    }
    
    void mouseMoved(int x, int y, int modif) override {
        if (ctrlDown(modif)) vvr::Scene::mouseMoved(x, y, modif);
        mPicker->mouseMoved(unproject(x, y), modif);
    }
    
    void mouseReleased(int x, int y, int modif) override {
        if (ctrlDown(modif)) vvr::Scene::mouseReleased(x, y, modif);
        mPicker->mouseReleased(unproject(x, y), modif);
    }

private:
    vvr::Colour colt, col1, col2;
    vvr::Axes *mAxes;
    tavli::Board *mBoard;
    tavli::PiecePicker *mPicker;
};

#endif
