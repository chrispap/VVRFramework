#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include "canvas.h"

#define APP_TITLE "Contour Editor"

using namespace std;
using namespace vvr;

/**
 * @brief The Simple2DScene class
 */
class ContourEditor : public vvr::Scene
{
public: 
    ContourEditor();
    const char* getName() const override { return APP_TITLE;}

protected:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;

private:
    void saveContoursToFile();

private:
    Canvas2D                m_canvas;
    float                   m_rad;
    vector<vector<Vec3d> >  m_pts;
};