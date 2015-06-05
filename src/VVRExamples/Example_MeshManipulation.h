#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include <vector>
#include <string>
#include <GeoLib.h>
#include <canvas.h>
#include <MathGeoLib.h>

using namespace vvr;
using namespace math;

class Scene3D : public vvr::Scene
{
public:
    Scene3D();
    const char* getName() const { return "3D Scene";}
    void keyEvent(unsigned char key, bool up, int modif) override;
    void arrowEvent(ArrowDir dir, int modif) override;

protected:
    void draw() override;
    void reset() override;
    void resize() override;
    void Tasks();

private:
    int             m_style_flag;
    Settings        m_settings;
    Canvas2D        m_canvas;
    Colour          m_obj_col;
    Mesh            m_model, m_model_edited;
    Vec3d           m_center_mass;
    vec             m_PCA_cen;
    vec             m_PCA_dir;
    Box3D           m_aabb;
    Plane           m_plane;
    float           m_plane_d;
    vector<int>     m_intersections;
};
