#define  NOMINMAX // Fixes a problem on windows
#include "GeoLab_Apallaktiki_02_YYYY.h"
#include "utils.h"
#include "canvas.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <MathGeoLib/MathGeoLib.h>

#define CONFIG_FILE_PATH "config/config_apallaktiki_02.txt"

#define FLAG_SHOW_AXES       1
#define FLAG_SHOW_AABB       2
#define FLAG_SHOW_WIRE       4
#define FLAG_SHOW_SOLID      8
#define FLAG_SHOW_NORMALS   16
#define CX					5
#define CY					5
#define CZ					5
#define CR					7

using namespace math;
using namespace vvr;
Vec3d operator-(Vec3d p1, Vec3d p2);

Simple3DScene::Simple3DScene()
{
    // Load settings.
    m_settings          = Settings(getBasePath() + CONFIG_FILE_PATH);
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
    const string objDir = getBasePath() + m_settings.getStr("obj_dir");
    const string objFile = getBasePath() +  m_settings.getStr("obj_file");
    m_icosahedron = Mesh(objDir, objFile, "", true);
}

void Simple3DScene::resize()
{
    // Making FIRST PASS static and initialing it to true we make
    // sure that the if block will be executed only once.
    static bool FLAG_FIRST_PASS = true;

    if (FLAG_FIRST_PASS)
    {

        
        for (int k = 0; k < 3; k++)
        {
            m_sphere_rad_1 = getSceneWidth() / 10;
            m_icosahedron.setBigSize(getSceneWidth() / 8);
            m_icosahedron.centerAlign();
            vector<Vec3d> &vertices = m_icosahedron.getVertices();
            vector<vvr::Triangle> &triangles = m_icosahedron.getTriangles();
            unsigned int vertices_number = vertices.size();
            unsigned int triangle_number = triangles.size();
            double radius = vertices[0].length();
            m_sphere_rad_1 = radius;
            m_icosahedron.setBigSize(getSceneWidth() / 8);
            m_icosahedron.centerAlign();
            
            
            for (unsigned vi = 0; vi < triangle_number; vi++)
            {
                unsigned int i1, i2, i3;
                Vec3d pt1 = triangles[vi].v1();
                Vec3d pt2 = triangles[vi].v2();
                Vec3d pt3 = triangles[vi].v3();
                Vec3d *pt4 = new Vec3d((pt1.x  + pt2.x), (pt1.y+ pt2.y) , (pt1.z  + pt2.z));
                Vec3d *pt5 = new Vec3d((pt1.x + pt3.x), (pt1.y + pt3.y) , (pt1.z + pt3.z));
                Vec3d *pt6 = new Vec3d((pt3.x + pt2.x), (pt3.y + pt2.y) , (pt3.z + pt2.z));

                pt4->scale(radius/pt4->length());
                pt5->scale(radius/ pt5->length());
                pt6->scale(radius / pt6->length());
                
                for (unsigned int j = 0; j < vertices_number; j++)
                {
                    if (vertices[j] == pt1)
                        i1 = j;
                    if (vertices[j] == pt2)
                        i2 = j;
                    if (vertices[j] == pt3)
                        i3 = j;
                }

                vertices.push_back(*pt4);
                vertices.push_back(*pt5);
                vertices.push_back(*pt6);
                unsigned a = vertices.size();

                triangles.push_back(vvr::Triangle(&vertices, a - 1, a - 2, a - 3));
                triangles.push_back(vvr::Triangle(&vertices, i1, a - 3, a - 2));
                triangles.push_back(vvr::Triangle(&vertices, i2, a - 1, a - 3));
                triangles.push_back(vvr::Triangle(&vertices, a - 1, a - 2, i3));
            }


            triangles.erase(triangles.begin(), triangles.begin() + triangle_number);
            
            Vec3d p1 = triangles[0].v1();
            Vec3d p2 = triangles[0].v2();
            Vec3d p3 = triangles[0].v3();
            Vec3d temp(p2.x / 2 + p3.x / 2, p2.y / 2 + p3.y / 2, p2.z / 2 + p3.z / 2);
            Vec3d p4(p1.x + 2. / 3 * (temp.x - p1.x), p1.y + 2. / 3 * (temp.y - p1.y), p1.z + 2. / 3 * (temp.z - p1.z));
            double surface_triangle = (p1 - temp).length()*(p2-p3).length()/2;
            double volume_polyhedron = triangles.size()*surface_triangle*p4.length()*sqrt(3.)/12;
            double volume_sphere = 4. / 3 * pi* radius*radius *radius;
            cout << triangles.size() << endl;
            cout << "precent volume loss\n" << (volume_sphere - volume_polyhedron) / volume_sphere << endl;

        }

        m_icosahedron.update();
        m_icosahedron1 = m_icosahedron;
        vector<Vec3d> &vertices_1 = m_icosahedron.getVertices();
        for (int i = 0; i < vertices_1.size(); i++)
        {
            vertices_1[i].scale(CR / vertices_1[i].length());
            vertices_1[i].x += CX;
            vertices_1[i].y += CY;
            vertices_1[i].z += CZ;
        }
        m_icosahedron1.update();
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
    if (m_style_flag & FLAG_SHOW_SOLID)     m_icosahedron1.draw(m_obj_col, SOLID);
    if (m_style_flag & FLAG_SHOW_WIRE)      m_icosahedron1.draw(Colour::black, WIRE);
    if (m_style_flag & FLAG_SHOW_NORMALS)   m_icosahedron1.draw(Colour::black, NORMALS);
    if (m_style_flag & FLAG_SHOW_AXES)      m_icosahedron1.draw(Colour::black, AXES);
    if (m_style_flag & FLAG_SHOW_AABB)      m_icosahedron1.draw(Colour::black, BOUND);
    double sphere_z = 0;
    math::vec plane_point(CX, CY, CZ);
    plane_point.Normalize();

    float d = (m_sphere_rad_1*m_sphere_rad_1 + CX*CX + CY*CY + CZ*CZ - CR*CR) / 2;
    Plane plane(plane_point,d);
    vvr::Sphere3D sphere(0, 0, sphere_z, m_sphere_rad_1, Colour(134, 100, 25));
    vvr::Sphere3D sphere1(CX, CY, CZ, CR, Colour(0, 0, 255));
    sphere.setSolidRender(false);
    sphere1.setSolidRender(false);
    float u = 50, v = 60;
    math::vec p1(plane.Point(-u, -v, math::vec(0, 0, 0)));
    math::vec p2(plane.Point(-u, v, math::vec(0, 0, 0)));
    math::vec p3(plane.Point(u, -v, math::vec(0, 0, 0)));
    math::vec p4(plane.Point(u, v, math::vec(0, 0, 0)));
    Triangle3D(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z, Colour::cyan).draw();
    Triangle3D(p4.x, p4.y, p4.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z, Colour::cyan).draw();
    //sphere1.draw();
    //sphere.draw();
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

Vec3d operator-(Vec3d p1, Vec3d p2)
{
    return Vec3d(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
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
