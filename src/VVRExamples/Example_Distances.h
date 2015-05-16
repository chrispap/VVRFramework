#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include "canvas.h"

/**
 * @brief The Simple2DScene class
 */
class DistancesScene : public vvr::Scene
{
public: 
    DistancesScene();
    const char* getName() const override;

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
