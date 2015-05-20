#define  NOMINMAX // Fixes a problem on windows
#include "Example_Template.h"
#include <utils.h>
#include <canvas.h>
#include <iostream>
#include <MathGeoLib/MathGeoLib.h>

#define FLAG_SHOW_AXES       1
#define FLAG_SHOW_AABB       2
#define FLAG_SHOW_WIRE       4
#define FLAG_SHOW_SOLID      8
#define FLAG_SHOW_NORMALS   16

TemplateScene::TemplateScene()
{
    m_perspective_proj = true;
    m_globRot_def = Vec3d();
    m_style_flag = FLAG_SHOW_SOLID | FLAG_SHOW_WIRE;
    m_obj_col = Colour(0x45, 0x45, 0x45);
    m_globRot = m_globRot_def;
    m_bg_col = Colour(0x76, 0x8E, 0x77);

    // Load 3D models.
    const string objDir = getBasePath() + "resources/obj/";
    const string objFile = getBasePath() + "resources/obj/suzanne.obj";
    m_mesh = Mesh::Make(objDir, objFile, "", true);
}

void TemplateScene::resize()
{
    static bool FIRST_PASS = true;

    if (FIRST_PASS)
    {
        m_mesh->setBigSize(getSceneWidth() / 2);
        m_mesh->centerAlign();
        m_mesh->update();
    }

    FIRST_PASS = false;
}

void TemplateScene::draw()
{
    // Draw mesh
    if (m_style_flag & FLAG_SHOW_SOLID)     m_mesh->draw(m_obj_col, SOLID);
    if (m_style_flag & FLAG_SHOW_WIRE)      m_mesh->draw(Colour::black, WIRE);
    if (m_style_flag & FLAG_SHOW_NORMALS)   m_mesh->draw(Colour::black, NORMALS);
    if (m_style_flag & FLAG_SHOW_AXES)      m_mesh->draw(Colour::black, AXES);
    if (m_style_flag & FLAG_SHOW_AABB)      m_mesh->draw(Colour::black, BOUND);
}

void TemplateScene::keyEvent(unsigned char key, bool up, int modif)
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
    try
    {
        return vvr::mainLoop(argc, argv, new TemplateScene);
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}
