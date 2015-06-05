#define  NOMINMAX // Fixes a problem on windows
#include "Example_MeshManipulation.h"
#include "utils.h"
#include "canvas.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <set>
#include <symmetriceigensolver3x3.h>
#include <MathGeoLib.h>

#define CONFIG_FILE_PATH "config/settings_meshManipulation.txt"

#define FLAG_SHOW_AXES       1
#define FLAG_SHOW_AABB       2
#define FLAG_SHOW_WIRE       4
#define FLAG_SHOW_SOLID      8
#define FLAG_SHOW_NORMALS   16
#define FLAG_SHOW_PLANE     32

void Task_1_FindCenterMass(vector<Vec3d> &vertices, Vec3d &cm);
void Task_2_AlignOriginTo(vector<Vec3d> &vertices, Vec3d &cm);
void Task_3_FindAABB(vector<Vec3d> &vertices, Box3D &aabb);
void Task_4_Draw_PCA(vec &center, vec &dir);
void Task_5_Intersect(vector<vvr::Triangle>& triangles, Plane &plane, vector<int> &intersection_indices);
void Task_5_Split(Mesh &mesh, Plane &plane);

void PCA(std::vector<Vec3d>& vertices, vec &center, vec &dir);

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
    m_style_flag = FLAG_SHOW_SOLID| FLAG_SHOW_AXES;
}

void Scene3D::resize()
{
    // Making FIRST PASS static and initialing it to true we make
    // sure that the if block will be executed only once.
    static bool FIRST_PASS = true;

    if (FIRST_PASS)
    {
        m_model.setBigSize(getSceneWidth()/2);
        Tasks();
        m_model.update();
    }

    FIRST_PASS = false;
}

void Scene3D::draw()
{
    // Draw plane
    if (m_style_flag & FLAG_SHOW_PLANE) {
        float u=40, v=40;
        math::vec p1(m_plane.Point(-u, -v, math::vec(0,0,0)));
        math::vec p2(m_plane.Point(-u,  v, math::vec(0,0,0)));
        math::vec p3(m_plane.Point( u, -v, math::vec(0,0,0)));
        math::vec p4(m_plane.Point( u,  v, math::vec(0,0,0)));
        Mesh m;
        m.getVertices().push_back(Vec3d(p1.x,p1.y,p1.z));
        m.getVertices().push_back(Vec3d(p2.x,p2.y,p2.z));
        m.getVertices().push_back(Vec3d(p3.x,p3.y,p3.z));
        m.getVertices().push_back(Vec3d(p4.x,p4.y,p4.z));
        m.getTriangles().push_back(vvr::Triangle(&m.getVertices(), 0,1,2));
        m.getTriangles().push_back(vvr::Triangle(&m.getVertices(), 1,2,3));
        m.update();

        m.draw(Colour(0x41, 0x14, 0xB3), (Style) ((int) SOLID | (int) NORMALS));
    }

    if (m_style_flag & FLAG_SHOW_SOLID)     m_model_edited.draw(m_obj_col, SOLID);
    if (m_style_flag & FLAG_SHOW_WIRE)      m_model_edited.draw(Colour::black, WIRE);
    if (m_style_flag & FLAG_SHOW_NORMALS)   m_model_edited.draw(Colour::black, NORMALS);
    if (m_style_flag & FLAG_SHOW_AXES)      m_model_edited.draw(Colour::black, AXES);

    //! Draw PCA line
    Task_4_Draw_PCA(m_PCA_cen, m_PCA_dir);

    // Draw center mass
    Point3D(m_center_mass.x, m_center_mass.y, m_center_mass.z, Colour::red).draw();

    // Draw AABB
    m_aabb.setColour(Colour::red);
    m_aabb.draw();

    // Draw intersecting triangles of model
    vector<vvr::Triangle> &triangles = m_model_edited.getTriangles();
    for (int i=0; i<m_intersections.size(); i++) {
        vvr::Triangle &t = triangles[m_intersections[i]];
        Triangle3D t3d(
            t.v1().x, t.v1().y, t.v1().z,
            t.v2().x, t.v2().y, t.v2().z,
            t.v3().x, t.v3().y, t.v3().z, Colour::green);
        t3d.draw();
    }

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
    case 'p': m_style_flag ^= FLAG_SHOW_PLANE; break;
        // NUMPAD
    case '0': setRot(m_globRot_def); break;
    case '4': setRot(Vec3d(  0,  -90,   0)); break;
    case '5': setRot(Vec3d(  0,   90,   0)); break;
    case '6': setRot(Vec3d(  0,  180,   0)); break;
    }

}

void Scene3D::arrowEvent(ArrowDir dir, int modif)
{
    math::vec n = m_plane.normal;

    if (dir == UP) m_plane_d += 1;
    if (dir == DOWN) m_plane_d -= 1;
    else if (dir == LEFT) n=math::float3x3::RotateY(DegToRad(1)).Transform(n);
    else if (dir == RIGHT) n=math::float3x3::RotateY(DegToRad(-1)).Transform(n);

    m_plane = Plane (n.Normalized(), m_plane_d);
    m_intersections.clear();
    //Task_5_Intersect(m_model_edited.getTriangles(), m_plane, m_intersections);
    m_model_edited = Mesh(m_model);
    Task_5_Split(m_model_edited, m_plane);
}

//! LAB Tasks

void Scene3D::Tasks()
{
    vector<vvr::Triangle>   &triangles = m_model_edited.getTriangles();
    vector<Vec3d>           &vertices  = m_model_edited.getVertices();

    //! Task 1
    Vec3d cm;
    Task_1_FindCenterMass(vertices, cm);
    m_center_mass = cm;

    //! Task 2
    Vec3d offs = cm;
    Task_2_AlignOriginTo(vertices, offs);
    m_center_mass = cm.sub(offs);

    //! Task 3
    Task_3_FindAABB(vertices, m_aabb);

    //! Task 4
    PCA(vertices, m_PCA_cen, m_PCA_dir);

    //! Task 5
    m_plane_d = 0;
    m_plane = Plane (vec(0, 1, 1).Normalized(), m_plane_d); // Define plane
    m_intersections.clear();
    Task_5_Intersect(triangles, m_plane, m_intersections);
    m_model_edited = Mesh(m_model);
    Task_5_Split(m_model_edited, m_plane);
}

void Task_1_FindCenterMass(vector<Vec3d> &vertices, Vec3d &cm)
{
    // - Breite to kentro mazas twn simeiwn  `vertices`
    // - Apothikeyste to apotelesma stin metavliti  `cm`

    const unsigned N = vertices.size();

    Vec3d sum;

    for (int i=0; i<N; i++) {
        sum.add(vertices[i]);
    }

    cm = sum.scale(1.0 / N);
}

void Task_2_AlignOriginTo(vector<Vec3d> &vertices, Vec3d &cm)
{
    // - Metatopiste to montelo esti wste to simeio `cm`
    // - na erthei sto (0,0,0).

    const unsigned N = vertices.size();

    for (int i=0; i<N; i++) {
        vertices[i].sub(cm);
    }

}

void Task_3_FindAABB(vector<Vec3d> &vertices, Box3D &aabb)
{
    // - Breite to Axis Aligned Bounding Box tou montelou
    // - To `aabb` orizetai apo 2 gwniaka simeia (to elaxisto kai to megisto).
    // -
    // - V_min: { aabb.x1, aabb.y1, aabb.z1 }
    // - V_max: { aabb.x2, aabb.y2, aabb.z2 }

    const unsigned N = vertices.size();

    vvr::Box b(vertices);
    aabb.x1 = b.min.x;
    aabb.y1 = b.min.y;
    aabb.z1 = b.min.z;
    aabb.x2 = b.max.x;
    aabb.y2 = b.max.y;
    aabb.z2 = b.max.z;
}

void Task_4_Draw_PCA(vec &center, vec &dir)
{
    // - Apeikoniste to kentro mazas kai ton principal axis tou PCA
    // - me ena simeio kai mia eytheia.

    Point3D     pt   (center.x, center.y, center.z, Colour::magenta);
    vec start = dir;
    vec end = dir;
    start *= -1000;
    end   *=  1000;
    start += center;
    end   += center;
    LineSeg3D   line (start.x, start.y, start.z, end.x, end.y, end.z, Colour::magenta);
    line.draw();
    pt.draw();
}

void Task_5_Intersect(vector<vvr::Triangle> &triangles, Plane &plane, vector<int> &intersection_indices)
{
    // - Brete ta trigwna pou temnontai me to epipedo `plane`.
    // - Kante ta push_back sto vector intersection_indices.

    const int N = triangles.size();

    for (int i=0; i < N; i++)
    {
        bool does_intersect = false;

        vvr::Triangle &t_vvr = triangles[i];

        vec v1 (t_vvr.v1().x, t_vvr.v1().y, t_vvr.v1().z);
        vec v2 (t_vvr.v2().x, t_vvr.v2().y, t_vvr.v2().z);
        vec v3 (t_vvr.v1().x, t_vvr.v3().y, t_vvr.v3().z);

        float a = plane.SignedDistance(v1);
        float b = plane.SignedDistance(v2);
        float c = plane.SignedDistance(v3);

        does_intersect = (a*b <= 0.f || a*c <= 0.f);

        if (does_intersect) {
            intersection_indices.push_back(i);
        }

    }

}

void Task_5_Split(Mesh &mesh, Plane &plane)
{
    vector<vvr::Triangle> &triangles = mesh.getTriangles();

    set<Vec3d*> vecs_R;
    set<Vec3d*> vecs_L;

    for (int i=0; i < triangles.size(); i++)
    {
        vvr::Triangle &t_vvr = triangles[i];
        math::vec v1 (t_vvr.v1().x, t_vvr.v1().y, t_vvr.v1().z);
        math::vec v2 (t_vvr.v2().x, t_vvr.v2().y, t_vvr.v2().z);
        math::vec v3 (t_vvr.v3().x, t_vvr.v3().y, t_vvr.v3().z);
        math::Triangle t(v1, v2, v3);

        bool ps1 = plane.IsOnPositiveSide(v1);
        bool ps2 = plane.IsOnPositiveSide(v2);
        bool ps3 = plane.IsOnPositiveSide(v3);

        if (ps1 && ps2 && ps3)
        {
            vecs_L.insert(const_cast<Vec3d*>(&t_vvr.v1()));
            vecs_L.insert(const_cast<Vec3d*>(&t_vvr.v2()));
            vecs_L.insert(const_cast<Vec3d*>(&t_vvr.v3()));
        }
        else if (!ps1 && !ps2 && !ps3)
        {
            vecs_R.insert(const_cast<Vec3d*>(&t_vvr.v1()));
            vecs_R.insert(const_cast<Vec3d*>(&t_vvr.v2()));
            vecs_R.insert(const_cast<Vec3d*>(&t_vvr.v3()));
        }
        else
        {
            triangles.erase(triangles.begin()+i--);
        }

    }

    // Split via translating to opposite diractions

    vec n = plane.normal * 2;
    Vec3d d = Vec3d (n.x, n.y, n.z);

    for (set<Vec3d*>::iterator vi=vecs_R.begin(); vi!=vecs_R.end(); ++vi) {
        (*vi)->sub(d);
    }

    for (set<Vec3d*>::iterator vi=vecs_L.begin(); vi!=vecs_L.end(); ++vi) {
        (*vi)->add(d);
    }

}

void PCA(vector<Vec3d>& vertices, vec &center, vec &dir)
{
    const int count = vertices.size();

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
