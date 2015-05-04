#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include <vector>
#include <string>
#include <GeoLib.h>
#include <canvas.h>
#include <MathGeoLib/MathGeoLib.h>

using namespace vvr;
using namespace math;

class Scene3D : public vvr::Scene
{
public:
    Scene3D();
    const char* getName() const { return "3D Scene";}
    void keyEvent(unsigned char key, bool up, int modif) override;

protected:
    void draw() override;
    void reset() override;
    void resize() override;

private:
    vvr::Mesh       m_model;
    vvr::Settings   m_settings;
    vvr::Colour     m_obj_col;
    int             m_style_flag;
    Canvas2D        m_canvas;

    /**********************************
     ** LAB TASK Objects             **
     **********************************/
    C2DTriangle     m_triangle;
    C2DLine         m_line;
    C2DCircle       m_circle;
    Plane           m_plane;

    Vec3d           m_center_mass;
    Vec3d           m_pca_cen;
    Vec3d           m_pca_dir;
    Box3D           m_AABB;
};
