#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include "canvas.h"

#define APP_TITLE "Simple 2D Drawing"

using namespace std;
using namespace vvr;

/**
 * @brief The Simple2DScene class
 */
class Simple2DScene : public vvr::Scene
{
public: 
    Simple2DScene();
    const char* getName() const override { return APP_TITLE;}

protected:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;

private:
    void mouse2pix(int &x, int &y);

private:
    Canvas2D                m_canvas;
    float                   m_rad;
    vector<vector<Vec3d> >  m_pts;
};
