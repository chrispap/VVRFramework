#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/canvas.h>

class Simple2DScene : public vvr::Scene
{
public:
    Simple2DScene();

    const char* getName() const override {
        return "Simple 2D Drawing";
    }

protected:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;

private:
    float m_rad;
    vvr::Canvas2D m_canvas;
};
