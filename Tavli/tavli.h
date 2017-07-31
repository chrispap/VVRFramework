#ifndef TAVLI_HEADER_240717
#define TAVLI_HEADER_240717

#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vector>

namespace tavli
{

    struct Piece : public vvr::Drawable
    {
        void draw() const override;
        vvr::Circle2D circle;
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

    struct Player 
    {

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
    };

}

#endif
