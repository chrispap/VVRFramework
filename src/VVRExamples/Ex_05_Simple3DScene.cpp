#define  NOMINMAX // Fixes a problem on windows
#include "Ex_05_Simple3DScene.h"
#include "utils.h"
#include "canvas.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <MathGeoLib/MathGeoLib.h>

#define FLAG_SHOW_AXES  1
#define FLAG_SHOW_WIRE   2
#define FLAG_SHOW_SOLID   4
#define FLAG_SHOW_NORMALS  8
#define FLAG_SHOW_AABB     16

using namespace math;
using namespace vvr;

Simple3DScene::Simple3DScene()
{
    m_style_flag = FLAG_SHOW_SOLID | FLAG_SHOW_WIRE;
    m_settings = Settings(getBasePath() + "config/settings_04.txt");
    m_scene_width = m_settings.getDbl("scene_width");
    m_scene_height = m_settings.getDbl("scene_height");
    m_bg_col = Colour(m_settings.getStr("color_bg"));
    m_perspective_proj = m_settings.getBool("perspective_proj");
    double def_rot_x = m_settings.getDbl("def_rot_x");
    double def_rot_y = m_settings.getDbl("def_rot_y");
    double def_rot_z = m_settings.getDbl("def_rot_z");
    m_globRotDef = Vec3d(def_rot_x, def_rot_y, def_rot_z);
    m_globRot = m_globRotDef;
    m_camera_dist = m_settings.getDbl("camera_dist");

    // Load
    const string objDir = getBasePath() + m_settings.getStr("obj_dir");
    const string objFile = getBasePath() +  m_settings.getStr("obj_file");

    m_mesh = Mesh(objDir, objFile, "");
    m_mesh.setBigSize(m_scene_width / 3);
    m_mesh.centerAlign();
}

void Simple3DScene::draw()
{
    drawAxes();

    if (m_style_flag & FLAG_SHOW_SOLID)
        m_mesh.draw(Colour(25,65,25), SOLID);
    if (m_style_flag & FLAG_SHOW_WIRE)
        m_mesh.draw(Colour::black, WIRE);
    if (m_style_flag & FLAG_SHOW_NORMALS)
        m_mesh.draw(Colour::black, NORMALS);
    if (m_style_flag & FLAG_SHOW_AXES)
        m_mesh.draw(Colour::black, AXES);
    if (m_style_flag & FLAG_SHOW_AABB)
        m_mesh.draw(Colour::black, BOUND);

    //Sphere3D sphere(0,0,0, m_scene_width/6, Colour(50,10,10));
    //sphere.draw();
}

//! Callbacks - UI

void Simple3DScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);

    key = tolower(key);

    if (key == 'a')       m_style_flag ^= FLAG_SHOW_AXES;
    else if (key == 'w')  m_style_flag ^= FLAG_SHOW_WIRE;
    else if (key == 's')  m_style_flag ^= FLAG_SHOW_SOLID;
    else if (key == 'n')  m_style_flag ^= FLAG_SHOW_NORMALS;
    else if (key == 'b')  m_style_flag ^= FLAG_SHOW_AABB;

}

void Simple3DScene::arrowEvent(ArrowDir dir, int modif)
{
    Scene::arrowEvent(dir, modif);
}

void Simple3DScene::reset()
{
    Scene::reset();
}
