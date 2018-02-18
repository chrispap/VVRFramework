#include <vvr/drawing.h>
#include <vvr/mesh.h>
#include <vvr/settings.h>
#include <vvr/utils.h>
#include <MathGeoLib.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <set>
#include "symmetriceigensolver3x3.h"

using namespace math;

//#define OBJ_FILENAME "pins.obj"
#define OBJ_FILENAME "vvrlab.obj"

void Task_1_FindCenterMass(std::vector<vec> &vertices, vec &cm);
void Task_2_FindAABB(std::vector<vec> &vertices, vvr::Aabb3D &aabb);
void Task_3_AlignOriginTo(std::vector<vec> &vertices, const vec &cm);
void Task_4_Draw_PCA(vec &center, vec &dir);
void Task_5_Intersect(std::vector<vvr::Triangle>& triangles, Plane &plane, std::vector<int> &intersection_indices);
void Task_5_Split(vvr::Mesh &mesh, Plane &plane);
void pca(std::vector<vec>& vertices, vec &center, vec &dir);
void FindSubMeshes(vvr::Mesh &mesh, vvr::Canvas &canvas);

class Mesh3DScene : public vvr::Scene
{
    enum {
        SHOW_AXES, SHOW_WIRE, SHOW_SOLID, SHOW_NORMALS, SHOW_PLANE, SHOW_AABB, SHOW_PCA,
        SHOW_INTERSECTIONS, SHOW_CM, SPLIT_INSTEAD_OF_INTERSECT, MODE_PARTITION,
    };

public:
    Mesh3DScene();
    const char* getName() const override { return "3D Scene"; }
    void keyEvent(unsigned char key, bool up, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;

private:
    void draw() override;
    void reset() override;
    void resize() override;
    vec Task_3_Pick_Origin();
    void Tasks();
    void printKeyboardShortcuts();

private:
    int m_flag;
    float m_plane_d;
    vvr::Canvas m_canvas;
    vvr::Colour m_obj_col;
    vvr::Mesh::Ptr m_model_original, m_model;
    vvr::Aabb3D m_aabb;
    math::vec m_center_mass;
    math::vec m_pca_cen;
    math::vec m_pca_dir;
    math::Plane m_plane;
    std::vector<int> m_intersections;
};

Mesh3DScene::Mesh3DScene()
{
    //! Load std::settings.
    vvr::Shape::LineWidth = 4;
    vvr::Shape::PointSize = 10;
    m_hide_log = false;
    m_perspective_proj = false;
    m_bg_col = vvr::Colour("768E77");
    m_obj_col = vvr::Colour("454545");
    const std::string objDir = vvr::getBasePath() + "resources/obj/";
    const std::string objFile = objDir + OBJ_FILENAME;
    m_model_original = vvr::Mesh::Make(objFile);
    reset();
}

void Mesh3DScene::reset()
{
    Scene::reset();

    //! Define plane
    m_plane_d = 0;
    m_plane = Plane(vec(0, 1, 1).Normalized(), m_plane_d);

    //! Define what will be vissible by default
    m_flag = 0;
    m_flag |= vvr_flag(SHOW_SOLID);
    m_flag |= vvr_flag(SHOW_WIRE);
    m_flag |= vvr_flag(MODE_PARTITION);
}

void Mesh3DScene::resize()
{
    //! By Making `first_pass` static and initializing it to true,
    //! we make sure that the if block will be executed only once.

    static bool first_pass = true;

    if (first_pass)
    {
        printKeyboardShortcuts();
        m_model_original->setBigSize(getSceneWidth() / 2);
        m_model_original->update();
        m_model = m_model_original;
        Tasks();
    }

    first_pass = false;
}

void Mesh3DScene::Tasks()
{
    //!//////////////////////////////////////////////////////////////////////////////////
    //! Task 1
    //!//////////////////////////////////////////////////////////////////////////////////

    m_center_mass = vec(-10, 0, 0);
    Task_1_FindCenterMass(m_model->getVertices(), m_center_mass);

    //!//////////////////////////////////////////////////////////////////////////////////
    //! Task 2
    //!//////////////////////////////////////////////////////////////////////////////////

    Task_2_FindAABB(m_model->getVertices(), m_aabb);

    //!//////////////////////////////////////////////////////////////////////////////////
    //! Task 3
    //!//////////////////////////////////////////////////////////////////////////////////

    Task_3_AlignOriginTo(m_model->getVertices(), Task_3_Pick_Origin());
    Task_1_FindCenterMass(m_model->getVertices(), m_center_mass);
    Task_2_FindAABB(m_model->getVertices(), m_aabb);
    m_model->update(true);

    //!//////////////////////////////////////////////////////////////////////////////////
    //! Task 4
    //!//////////////////////////////////////////////////////////////////////////////////

    pca(m_model->getVertices(), m_pca_cen, m_pca_dir);

    //!//////////////////////////////////////////////////////////////////////////////////
    //! Homework
    //!//////////////////////////////////////////////////////////////////////////////////

    float t = vvr::getSeconds();
    FindSubMeshes(*m_model, m_canvas);
    t = vvr::getSeconds() - t;
    std::cout << "Partitioned in " << t << " sec" << std::endl;

    //!//////////////////////////////////////////////////////////////////////////////////
    //! Task 5
    //!//////////////////////////////////////////////////////////////////////////////////

    m_model_original = m_model;

    if (!vvr_flag_test(m_flag, SPLIT_INSTEAD_OF_INTERSECT))
    {
        Task_5_Intersect(m_model->getTriangles(), m_plane, m_intersections);
    }
    else
    {
        m_intersections.clear();
        m_model = vvr::Mesh::Make(*m_model_original);
        Task_5_Split(*m_model, m_plane);
    }
}

void Mesh3DScene::draw()
{
    //! Draw plane
    if (vvr_flag_test(m_flag, SHOW_PLANE)) {
        vvr::Colour colPlane(0x41, 0x14, 0xB3);
        float u = 20, v = 20;
        math::vec p0(m_plane.Point(-u, -v, math::vec(0, 0, 0)));
        math::vec p1(m_plane.Point(-u, v, math::vec(0, 0, 0)));
        math::vec p2(m_plane.Point(u, -v, math::vec(0, 0, 0)));
        math::vec p3(m_plane.Point(u, v, math::vec(0, 0, 0)));
        vvr::Triangle3D(math::Triangle(p0, p1, p2), colPlane).draw();
        vvr::Triangle3D(math::Triangle(p2, p1, p3), colPlane).draw();
    }

    if (vvr_flag_test(m_flag, SHOW_SOLID)) m_model->draw(m_obj_col, vvr::SOLID);
    if (vvr_flag_test(m_flag, SHOW_WIRE)) m_model->draw(vvr::black, vvr::WIRE);
    if (vvr_flag_test(m_flag, SHOW_NORMALS)) m_model->draw(vvr::black, vvr::NORMALS);
    if (vvr_flag_test(m_flag, SHOW_AXES)) m_model->draw(vvr::black, vvr::AXES);

    //! Draw pca line
    if (vvr_flag_test(m_flag, SHOW_PCA)) {
        Task_4_Draw_PCA(m_pca_cen, m_pca_dir);
    }

    //! Draw center mass
    if (vvr_flag_test(m_flag, SHOW_CM)) {
        vvr::Point3D(m_center_mass.x, m_center_mass.y, m_center_mass.z, vvr::red).draw();
    }

    //! Draw AABB
    if (vvr_flag_test(m_flag, SHOW_AABB)) {
        m_aabb.colour = vvr::black;
        m_aabb.setTransparency(1);
        m_aabb.draw();
    }

    //! Draw intersecting triangles of model
    if (vvr_flag_test(m_flag, SHOW_INTERSECTIONS)) {
        std::vector<vvr::Triangle> &triangles = m_model->getTriangles();
        for (int i = 0; i < m_intersections.size(); i++) {
            vvr::Triangle &t = triangles[m_intersections[i]];
            vvr::Triangle3D t3d(
                t.v1().x, t.v1().y, t.v1().z,
                t.v2().x, t.v2().y, t.v2().z,
                t.v3().x, t.v3().y, t.v3().z,
                vvr::green);
            t3d.draw();
        }
    }

    m_canvas.draw();
}

void Mesh3DScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (vvr_flag_test(m_flag, MODE_PARTITION))
    {
        if (dir == vvr::UP) m_canvas.next();
        else if (dir == vvr::DOWN) m_canvas.prev();
        return;
    }

    math::vec n = m_plane.normal;
    if (dir == vvr::UP) m_plane_d += 1;
    else if (dir == vvr::DOWN) m_plane_d -= 1;
    else if (dir == vvr::LEFT) n = math::float3x3::RotateY(DegToRad(1)).Transform(n);
    else if (dir == vvr::RIGHT) n = math::float3x3::RotateY(DegToRad(-1)).Transform(n);
    m_plane = Plane(n.Normalized(), m_plane_d);

    if (!vvr_flag_test(m_flag, SPLIT_INSTEAD_OF_INTERSECT))
    {
        Task_5_Intersect(m_model_original->getTriangles(), m_plane, m_intersections);
    }
    else
    {
        m_intersections.clear();
        m_model = vvr::Mesh::Make(*m_model_original);
        Task_5_Split(*m_model, m_plane);
    }
}

void Mesh3DScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    key = tolower(key);

    switch (key)
    {
        vvr_flag_toggle(m_flag, 's', SHOW_SOLID);
        vvr_flag_toggle(m_flag, 'w', SHOW_WIRE);
        vvr_flag_toggle(m_flag, 'n', SHOW_NORMALS);
        vvr_flag_toggle(m_flag, 'a', SHOW_AXES);
        vvr_flag_toggle(m_flag, 'p', SHOW_PLANE);
        vvr_flag_toggle(m_flag, 'b', SHOW_AABB);
        vvr_flag_toggle(m_flag, 'c', SHOW_PCA);
        vvr_flag_toggle(m_flag, 'i', SHOW_INTERSECTIONS);
        vvr_flag_toggle(m_flag, 'm', SHOW_CM);
        vvr_flag_toggle(m_flag, 'x', SPLIT_INSTEAD_OF_INTERSECT);
    }
}

void Mesh3DScene::printKeyboardShortcuts()
{
    std::cout << "Keyboard shortcuts:"
        << std::endl << "'?' => This shortcut list:"
        << std::endl << "'s' => RENDER SOLID"
        << std::endl << "'w' => RENDER WIREFRAME"
        << std::endl << "'n' => SHOW NORMALS"
        << std::endl << "'a' => SHOW AXES"
        << std::endl << "'p' => SHOW PLANE"
        << std::endl << "'b' => SHOW AABB"
        << std::endl << "'c' => SHOW PCA LINE"
        << std::endl << "'i' => SHOW INTERSECTING TRIANGLES"
        << std::endl << "'m' => SHOW CENTER OF MASS"
        << std::endl << "'x' => SPLIT_INSTEAD_OF_INTERSECT"
        << std::endl << std::endl;
}

void pca(std::vector<vec>& vertices, vec &center, vec &dir)
{
    const int count = vertices.size();

    float w0 = 0;
    float x0 = 0, y0 = 0, z0 = 0;
    float x2 = 0, y2 = 0, z2 = 0, xy = 0, yz = 0, xz = 0;
    float dx2, dy2, dz2, dxy, dxz, dyz;
    float det[9];

    for (int i = 0; i < count; i++)
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
    w0 = (float)count;

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

    /* Searching for a eigenstd::vector of det corresponding to the minimal eigenvalue */
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

//! LAB Tasks

void Task_1_FindCenterMass(std::vector<vec> &vertices, vec &cm)
{
    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK:
    //!
    //!  - Breite to kentro mazas twn simeiwn `vertices`.
    //!  - Apothikeyste to apotelesma stin metavliti `cm`.
    //!
    //!//////////////////////////////////////////////////////////////////////////////////

    const unsigned N = vertices.size();

    cm = vec(0,0,0);

    for (int i = 0; i < N; i++) {
        cm += vertices[i];
    }

    cm /= N;
}

void Task_2_FindAABB(std::vector<vec> &vertices, vvr::Aabb3D &aabb)
{
    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK:
    //!
    //!  - Breite to Axis Aligned Bounding Box tou montelou
    //!
    //! HINTS:
    //!
    //!  - To `aabb` orizetai apo 2 gwniaka simeia. (V_min, V_max)
    //!  - V_min: { aabb.x1, aabb.y1, aabb.z1 }
    //!  - V_max: { aabb.x2, aabb.y2, aabb.z2 }
    //!
    //!//////////////////////////////////////////////////////////////////////////////////

    aabb = vvr::Aabb3D(vertices);
}

vec Mesh3DScene::Task_3_Pick_Origin()
{
    return vec(0.5f * (m_aabb.MinX() + m_aabb.MaxX()),
               0.5f * (m_aabb.MinY() + m_aabb.MaxY()),
               0.5f * (m_aabb.MinZ() + m_aabb.MaxZ()));
    return m_center_mass;
}

void Task_3_AlignOriginTo(std::vector<vec> &vertices, const vec &cm)
{
    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK:
    //!
    //!  - Metatopiste to montelo esti wste to simeio `cm` na erthei sto (0,0,0).
    //!
    //!//////////////////////////////////////////////////////////////////////////////////

    const unsigned N = vertices.size();

    for (int i = 0; i < N; i++) {
        vertices[i] -= cm;
    }

}

void Task_4_Draw_PCA(vec &center, vec &dir)
{
    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK:
    //!
    //!  - Apeikoniste to kentro mazas kai ton Principal Axis tou PCA.
    //!    Gia tin apeikonisi, xreiazeste ena simeio kai mia eytheia.
    //!
    //! HINTS:
    //!  - Auti i synartisi kaleitai mesa apo tin `Mesh3DScene::draw()`.
    //!    Ara, mporeite na kalesete amesa tis metodous draw ton diaforwn antikeimenwn
    //!
    //!//////////////////////////////////////////////////////////////////////////////////

    vvr::Point3D pt(center.x, center.y, center.z, vvr::magenta);
    vec start = dir;
    vec end = dir;
    start *= -1000;
    end *= 1000;
    start += center;
    end += center;
    vvr::LineSeg3D line(start.x, start.y, start.z, end.x, end.y, end.z, vvr::magenta);
    line.draw();
    pt.draw();
}

void Task_5_Intersect(std::vector<vvr::Triangle> &triangles, Plane &plane, std::vector<int> &intersection_indices)
{
    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK:
    //!
    //!  - Brete ta trigwna pou temnontai me to epipedo `plane`.
    //!  - Kante ta push_back sto std::vector intersection_indices.
    //!
    //!//////////////////////////////////////////////////////////////////////////////////

    intersection_indices.clear();

    const int N = triangles.size();

    for (int i = 0; i < N; i++)
    {
        vvr::Triangle &t_vvr = triangles[i];

        vec v1(t_vvr.v1().x, t_vvr.v1().y, t_vvr.v1().z);
        vec v2(t_vvr.v2().x, t_vvr.v2().y, t_vvr.v2().z);
        vec v3(t_vvr.v3().x, t_vvr.v3().y, t_vvr.v3().z);

        float a = plane.SignedDistance(v1);
        float b = plane.SignedDistance(v2);
        float c = plane.SignedDistance(v3);

        const bool intersects = (a*b <= 0 || a*c <= 0);

        if (intersects) {
            intersection_indices.push_back(i);
        }
    }
}

void Task_5_Split(vvr::Mesh &mesh, Plane &plane)
{
    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK:
    //!
    //!  - Kopste to antikeimeno sta 2. (Odigies tin ora tou ergasthriou)
    //!
    //!//////////////////////////////////////////////////////////////////////////////////

    std::vector<vvr::Triangle> &triangles = mesh.getTriangles();

    std::set<vec*> verts_right;
    std::set<vec*> verts_left;

    for (int i = 0; i < triangles.size(); i++)
    {
        vvr::Triangle &t_vvr = triangles[i];
        math::vec v1(t_vvr.v1().x, t_vvr.v1().y, t_vvr.v1().z);
        math::vec v2(t_vvr.v2().x, t_vvr.v2().y, t_vvr.v2().z);
        math::vec v3(t_vvr.v3().x, t_vvr.v3().y, t_vvr.v3().z);
        math::Triangle t(v1, v2, v3);

        bool ps1 = plane.IsOnPositiveSide(v1);
        bool ps2 = plane.IsOnPositiveSide(v2);
        bool ps3 = plane.IsOnPositiveSide(v3);

        if (ps1 && ps2 && ps3)
        {
            verts_left.insert(const_cast<vec*>(&t_vvr.v1()));
            verts_left.insert(const_cast<vec*>(&t_vvr.v2()));
            verts_left.insert(const_cast<vec*>(&t_vvr.v3()));
        }
        else if (!ps1 && !ps2 && !ps3)
        {
            verts_right.insert(const_cast<vec*>(&t_vvr.v1()));
            verts_right.insert(const_cast<vec*>(&t_vvr.v2()));
            verts_right.insert(const_cast<vec*>(&t_vvr.v3()));
        }
        else
        {
            triangles.erase(triangles.begin() + i--);
        }

    }

    // Split via translating to opposite diractions

    vec n = plane.normal;
    vec d = vec(n.x, n.y, n.z);

    for (std::set<vec*>::iterator vi = verts_right.begin(); vi != verts_right.end(); ++vi) {
        **vi -= d;
    }

    for (std::set<vec*>::iterator vi = verts_left.begin(); vi != verts_left.end(); ++vi) {
        **vi += d;
    }
}

void FindSubMeshes(vvr::Mesh &mesh, vvr::Canvas &canvas)
{
    const std::vector<vvr::Triangle> &tris = mesh.getTriangles();
    const std::vector<vec> &vecs = mesh.getVertices();

    int NUM_OF_VECS = vecs.size();
    int NUM_OF_PARTS = 0;

    int i = 0;
    std::vector<std::set<int> > vectris(vecs.size());
    for (std::vector<vvr::Triangle>::const_iterator ti = tris.cbegin();
        ti != tris.cend();
        ++ti)
    {
        vectris[ti->vi1].insert(i);
        vectris[ti->vi2].insert(i);
        vectris[ti->vi3].insert(i);
        ++i;
    }

    std::set<int> v_tbc;

    for (int i = 0; i < vecs.size(); i++) {
        v_tbc.insert(i);
    }

    while (!v_tbc.empty())
    {
        std::set<int> v_part;
        std::set<int> v_step;
        v_step.insert(*v_tbc.begin());
        unsigned v_part_size = 0;

        do
        {
            std::set<int> v_step_new;
            for (auto vi : v_step)
            {
                for (auto ti : vectris[vi])
                {
                    const auto &tti = tris.at(ti);
                    if (v_part.find(tti.vi1) == v_part.end()) v_step_new.insert(tti.vi1);
                    if (v_part.find(tti.vi2) == v_part.end()) v_step_new.insert(tti.vi2);
                    if (v_part.find(tti.vi3) == v_part.end()) v_step_new.insert(tti.vi3);
                }
            }
            v_step = v_step_new;
            v_part_size = v_part.size();
            v_part.insert(std::begin(v_step), std::end(v_step));
        } while (v_part_size != v_part.size());

        std::vector<vec> vs;
        for (auto v : v_part) {
            vs.push_back(vecs.at(v));
            v_tbc.erase(v);
        }

        vvr::Aabb3D* bb = new vvr::Aabb3D(vs);

        if (bb->MaxX() - bb->MinX() < 1 ||
            bb->MaxY() - bb->MinY() < 1 ||
            bb->MaxZ() - bb->MinZ() < 1)
        {

        }
        else
        {
            bb->colour = vvr::orange;
            bb->setTransparency(0.40);
            canvas.add(bb);
            canvas.newFrame(true);
            NUM_OF_PARTS++;
        }
    }

    vvr_echo(NUM_OF_VECS);
    vvr_echo(NUM_OF_PARTS);
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(Mesh3DScene)
#endif
