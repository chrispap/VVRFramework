#ifndef TAVLI_HEADER_240717
#define TAVLI_HEADER_240717

#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vector>

namespace tavli
{

    struct Piece : public vvr::Drawable
    {
        Piece();
        void draw() const override;
        vvr::Circle2D cir;
    };

    struct Region : public vvr::Drawable
    {
        Region();
        void draw() const override;
        vvr::Triangle2D tri;
    };

    struct Board : public vvr::Drawable
    {
        Board(float w, float h);
        void draw() const override;
        std::vector<Piece*> pieces;
        std::vector<Region*> regions;
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
        vvr::Canvas mCanvas;
        vvr::GlobalAxes *mAxes;
        Board *mBoard;
    };

}

#endif