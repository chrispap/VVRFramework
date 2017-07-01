#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/canvas.h>

class TutorialScene : public vvr::Scene
{
public: 
    TutorialScene();

    const char* getName() const override {
        return "VVRFramework Tutorial";
    }

protected:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;

private:
    vvr::Canvas2D m_canvas;
};
