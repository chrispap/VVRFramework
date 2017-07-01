#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/canvas.h>

#define APP_TITLE "Contour Editor"

using namespace std;
using namespace vvr;

/**
 * @brief The Simple2DScene class
 */
class ContourEditorScene : public vvr::Scene
{
public: 
    ContourEditorScene();
    const char* getName() const override { return APP_TITLE;}

protected:
    void draw() override;
    void reset() override;
    void resize() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void arrowEvent(ArrowDir dir, int modif) override;

private:
    void saveContoursToFile();
    void loadContoursFromFile(string filename);
    void pixelCoordsToSceneCoords(float &x, float &y);

private:
    vector<vector<vec> >    m_pts;
    vector<float>           m_heights;
    Canvas2D                m_canvas;
    float                   m_rad;
    bool                    b_render_3D, b_show_pts;
    int                     m_active_contour;
    math::Frustum           m_frustum;
};
