#define  NOMINMAX // Fixes a problem on windows
#include "GeoLab_Apallaktiki_02_XXXX.h"
#include "utils.h"
#include "canvas.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <MathGeoLib/MathGeoLib.h>

#define CONFIG_FILE_PATH "../../config/config_apallaktiki_02.txt"

#define FLAG_SHOW_AXES       1
#define FLAG_SHOW_AABB       2
#define FLAG_SHOW_WIRE       4
#define FLAG_SHOW_SOLID      8
#define FLAG_SHOW_NORMALS   16

using namespace math;
using namespace vvr;

Simple3DScene::Simple3DScene()
{
    // Load settings.
    m_settings          = Settings(getExePath() + CONFIG_FILE_PATH);
    m_bg_col            = Colour(m_settings.getStr("color_bg"));
    m_obj_col           = Colour(m_settings.getStr("color_obj"));
    m_perspective_proj  = m_settings.getBool("perspective_proj");
    m_style_flag        = FLAG_SHOW_SOLID | FLAG_SHOW_WIRE;

    // Scene rotation.
    const double def_rot_x = m_settings.getDbl("def_rot_x");
    const double def_rot_y = m_settings.getDbl("def_rot_y");
    const double def_rot_z = m_settings.getDbl("def_rot_z");
    m_globRot_def = Vec3d(def_rot_x, def_rot_y, def_rot_z);
    m_globRot = m_globRot_def;

    // Load 3D models.
    const string objDir = getExePath() + m_settings.getStr("obj_dir");
    const string objFile = getExePath() +  m_settings.getStr("obj_file");
    m_icosahedron = Mesh(objDir, objFile, "", true);
}

void Simple3DScene::resize()
{
    // Making FIRST PASS static and initialing it to true we make
    // sure that the if block will be executed only once.
    static bool FLAG_FIRST_PASS = true;

    if (FLAG_FIRST_PASS)
    {
        m_sphere_rad = getSceneWidth()/10;
        m_icosahedron.setBigSize(getSceneWidth()/8);
        m_icosahedron.centerAlign();

        vector<Vec3d> &vertices = m_icosahedron.getVertices();

        for (unsigned vi=0 ; vi<vertices.size() ; vi++)
        {
            Vec3d &vertex = vertices[vi];

            // Here you can process the vertices of the icosahedron model
            //...
            //...
            //...

        }

        m_icosahedron.update();

    }

    FLAG_FIRST_PASS = false;
}

void Simple3DScene::draw()
{
    if (m_style_flag & FLAG_SHOW_SOLID)     m_icosahedron.draw(m_obj_col, SOLID);
    if (m_style_flag & FLAG_SHOW_WIRE)      m_icosahedron.draw(Colour::black, WIRE);
    if (m_style_flag & FLAG_SHOW_NORMALS)   m_icosahedron.draw(Colour::black, NORMALS);
    if (m_style_flag & FLAG_SHOW_AXES)      m_icosahedron.draw(Colour::black, AXES);
    if (m_style_flag & FLAG_SHOW_AABB)      m_icosahedron.draw(Colour::black, BOUND);

    double sphere_z = 0;

    vvr::Sphere3D sphere(0, 0, sphere_z, m_sphere_rad, Colour(134, 100, 25));
    sphere.setSolidRender(false);
    sphere.draw();
}

void Simple3DScene::keyEvent(unsigned char key, bool up, int modif)
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
        return vvr::mainLoop(argc, argv, new Simple3DScene);
    }
    catch (std::string exc) {
        std::cerr << exc << std::endl;
        return 1;
    }
}
