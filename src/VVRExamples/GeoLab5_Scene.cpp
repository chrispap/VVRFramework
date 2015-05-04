#define  NOMINMAX // Fixes a problem on windows
#include "GeoLab5_Scene.h"
#include "utils.h"
#include "canvas.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <symmetriceigensolver3x3.h>
#include <MathGeoLib/MathGeoLib.h>

#define CONFIG_FILE_PATH "config/settings_geolab5.txt"

#define FLAG_SHOW_AXES       1
#define FLAG_SHOW_AABB       2
#define FLAG_SHOW_WIRE       4
#define FLAG_SHOW_SOLID      8
#define FLAG_SHOW_NORMALS   16

using namespace math;
using namespace vvr;

void Task_1_FindCenterMass(vector<Vec3d> &vertices, Vec3d &cm);
void Task_2_AlignTo(vector<Vec3d> &vertices, Vec3d &cm);
void Task_3_FindAABB(vector<Vec3d> &vertices, Box3D &aabb);
void Task_4_PCA(std::vector<Vec3d>& vertices, Vec3d &center, Vec3d &dir);

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
    m_style_flag = FLAG_SHOW_WIRE | FLAG_SHOW_AXES;
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
        m_model.setBigSize(getSceneWidth()/3);
        vector<Vec3d> &vertices = m_model.getVertices();

        //////////////////////////////////////////////////////////////////////////////////
        //! Execute Tasks
        //////////////////////////////////////////////////////////////////////////////////

        Vec3d cm, offs;

        // Task 1
        Task_1_FindCenterMass(vertices, cm);

        // Task 2
        offs = cm;
        Task_2_AlignTo(vertices, offs);
        m_center_mass = cm.sub(offs);

        // Task 3
        Task_3_FindAABB(vertices, m_AABB);

        Task_4_PCA(vertices, m_pca_cen, m_pca_dir);

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

    //! Draw PCA line
    {
        Vec3d e = m_pca_dir;
        e.normalize().scale(m_model.getBox().getMaxSize()).add(m_pca_cen);
        LineSeg3D(m_pca_cen.x, m_pca_cen.y, m_pca_cen.z, e.x, e.y, e.z, Colour::magenta).draw();
        Point3D(m_pca_cen.x, m_pca_cen.y, m_pca_cen.z, Colour::magenta).draw();
    }

    // Draw center mass
    Point3D(m_center_mass.x, m_center_mass.y, m_center_mass.z, Colour::red).draw();

    // Draw AABB
    m_AABB.setColour(Colour::red);
    m_AABB.draw();
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

void Task_1_FindCenterMass(vector<Vec3d> &vertices, Vec3d &cm)
{
    const unsigned N = vertices.size();

    Vec3d sum;
    for (int i=0; i < N; i++) {
        Vec3d &vertex = vertices[i];
        sum.add(vertex);
    }
    sum.scale( 1.0 / N );
    cm = sum;
}

void Task_2_AlignTo(vector<Vec3d> &vertices, Vec3d &cm)
{
    const unsigned N = vertices.size();

    for (int i=0; i < N; i++) {
        Vec3d &vertex = vertices[i];
        vertex.sub(cm);
    }
}

void Task_3_FindAABB(vector<Vec3d> &vertices, Box3D &aabb)
{
    Box box(vertices);
    aabb.x1 = box.min.x;
    aabb.y1 = box.min.y;
    aabb.z1 = box.min.z;
    aabb.x2 = box.max.x;
    aabb.y2 = box.max.y;
    aabb.z2 = box.max.z;
//<<<
    const unsigned N = vertices.size();

    for (int i=0; i < N; i++) {
        Vec3d &vertex = vertices[i];

    }

}

void Task_4_PCA(std::vector<Vec3d>& vertices, Vec3d &center, Vec3d &dir)
{
    //Position is a x,y,z struct
    int count = vertices.size();

    float w0 = 0;
    float x0 = 0, y0 = 0, z0 = 0;
    float x2 = 0, y2 = 0, z2 = 0, xy = 0, yz = 0, xz = 0;
    float dx2, dy2, dz2, dxy, dxz, dyz;
    float det[9];

    for(int i = 0; i < count; i++ )
    {
        float x = vertices[i].x;
        float y = vertices[i].y;
        float z = vertices[i].z;

        x2 += x * x;
        xy += x * y;
        xz += x * z;
        y2 += y * y;
        yz += y * z;
        z2 += z * z;
        x0 += x;
        y0 += y;
        z0 += z;
    }
    w0 = (float) count;

    x2 /= w0;
    xy /= w0;
    xz /= w0;
    y2 /= w0;
    yz /= w0;
    z2 /= w0;

    x0 /= w0;
    y0 /= w0;
    z0 /= w0;

    dx2 = x2 - x0 * x0;
    dxy = xy - x0 * y0;
    dxz = xz - x0 * z0;
    dy2 = y2 - y0 * y0;
    dyz = yz - y0 * z0;
    dz2 = z2 - z0 * z0;

    det[0] = dz2 + dy2;
    det[1] = -dxy;
    det[2] = -dxz;
    det[3] = det[1];
    det[4] = dx2 + dz2;
    det[5] = -dyz;
    det[6] = det[2];
    det[7] = det[5];
    det[8] = dy2 + dx2;

    /* Searching for a eigenvector of det corresponding to the minimal eigenvalue */
    gte::SymmetricEigensolver3x3<float> solver;
    std::array<float, 3> eval;
    std::array<std::array<float, 3>, 3> evec;
    solver(det[0], det[1], det[2], det[4], det[5], det[8], true, 1, eval, evec);

    center.x = x0;
    center.y = y0;
    center.z = z0;

    dir.x = evec[0][0];
    dir.y = evec[0][1];
    dir.z = evec[0][2];
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
