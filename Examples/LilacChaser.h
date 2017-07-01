#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/canvas.h>
#include <vector>

class LilacChaserScene : public vvr::Scene
{
public:
    LilacChaserScene();

    const char* getName() const override {
        return "Lilac Chaser Illusion";
    }

protected:
    void draw() override;
    void reset() override;
    bool idle() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;

private:
    vvr::Canvas2D m_canvas;
    std::vector<vvr::Circle2D*> m_circles;

private:
    const float R = 120;
    const float r = 20;
    const unsigned N = 12;
    const vvr::Colour col_lilac = vvr::Colour("CDA9CD");
};
