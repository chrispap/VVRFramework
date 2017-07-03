#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/drawing.h>

class TutorialScene : public vvr::Scene
{
public:
    TutorialScene();

    const char* getName() const override {
        return "UNIVERSITY OF PATRAS - VVR GROUP - COMPUTATIONAL GEOMETRY LAB";
    }

    vvr::Canvas2D canvas;
    std::vector<C2DPoint> pts;
    
protected:
    void draw() override;
    void mousePressed(int x, int y, int modif) override;
};
