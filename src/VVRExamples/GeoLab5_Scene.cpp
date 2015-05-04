#define  NOMINMAX // Fixes a problem on windows
#include "GeoLab5_Scene.h"
#include "utils.h"
#include "canvas.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <MathGeoLib/MathGeoLib.h>

#define CONFIG_FILE_PATH "config/settings_geolab5.txt"

#define FLAG_SHOW_AXES       1
#define FLAG_SHOW_AABB       2
#define FLAG_SHOW_WIRE       4
#define FLAG_SHOW_SOLID      8
#define FLAG_SHOW_NORMALS   16

using namespace math;
using namespace vvr;

Scene3D::Scene3D()
{
    // Load settings.
    m_settings          = Settings(getBasePath() + CONFIG_FILE_PATH);
    m_bg_col            = Colour(m_settings.getStr("color_bg"));
    m_obj_col           = Colour(m_settings.getStr("color_obj"));
    m_perspective_proj  = m_settings.getBool("perspective_proj");

    // Scene rotation.
    const double def_rot_x = m_settings.getDbl("def_rot_x");
    const double def_rot_y = m_settings.getDbl("def_rot_y");
    const double def_rot_z = m_settings.getDbl("def_rot_z");
    m_globRot_def = Vec3d(def_rot_x, def_rot_y, def_rot_z);
    m_globRot = m_globRot_def;

    // Load 3D models.
    const string objDir = getBasePath() + m_settings.getStr("obj_dir");
    const string objFile = getBasePath() +  m_settings.getStr("obj_file");
    m_model = Mesh(objDir, objFile, "", true);

    // Reset
    reset();
}

void Scene3D::reset()
{
    Scene::reset();
    m_style_flag = FLAG_SHOW_SOLID | FLAG_SHOW_AXES | FLAG_SHOW_AABB;

    m_triangle   = C2DTriangle (C2DPoint(-290, -225), C2DPoint(-435, 35), C2DPoint(-222, 5));
    m_line       = C2DLine     (C2DPoint(-235, 220), C2DPoint(215, 150));
    m_circle     = C2DCircle   (C2DPoint(200, -100), 110);
    m_plane      = Plane       (vec(0, 1, 1).Normalized(), 0);
}

void Scene3D::resize()
{
    // Making FIRST PASS static and initialing it to true we make
    // sure that the if block will be executed only once.
    static bool FIRST_PASS = true;

    if (FIRST_PASS)
    {
        m_model.setBigSize(getSceneWidth()/2);
        m_model.centerAlign();
        vector<Vec3d> &vertices = m_model.getVertices();
        //...
        //...
        //...
        m_model.update();
    }

    FIRST_PASS = false;
}

void Scene3D::draw()
{
    if (m_style_flag & FLAG_SHOW_SOLID)     m_model.draw(m_obj_col, SOLID);
    if (m_style_flag & FLAG_SHOW_WIRE)      m_model.draw(Colour::black, WIRE);
    if (m_style_flag & FLAG_SHOW_NORMALS)   m_model.draw(Colour::black, NORMALS);
    if (m_style_flag & FLAG_SHOW_AXES)      m_model.draw(Colour::black, AXES);
    if (m_style_flag & FLAG_SHOW_AABB)      m_model.draw(Colour::black, BOUND);
}

void Scene3D::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    key = tolower(key);

    switch (key) 
    {
    case 'a': m_style_flag ^= FLAG_SHOW_AXES; break;
    case 'w': m_style_flag ^= FLAG_SHOW_WIRE; break;
    case 's': m_style_flag ^= FLAG_SHOW_SOLID; break;
    case 'n': m_style_flag ^= FLAG_SHOW_NORMALS; break;
    case 'b': m_style_flag ^= FLAG_SHOW_AABB; break;
    }

}

int main(int argc, char* argv[])
{
    try {
        return vvr::mainLoop(argc, argv, new Scene3D);
    }
    catch (std::string exc) {
        std::cerr << exc << std::endl;
        return 1;
    }
    catch(...) {
        std::cerr << "Unknown exception" << std::endl;
        return 1;
    }
}
