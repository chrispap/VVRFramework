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

//! MACROS used for toggling and testing bitwise flags.
#define FLAG(x) (1<<(x))
#define FLAG_ON(v,f) (v & FLAG(f))
#define FLAG_TOGGLE(v,c,f) case c: v ^= FLAG(f); std::cout \
    << #f << " = " << (FLAG_ON(v,f) ? "ON" : "OFF") \
    << std::endl; break

void Task_1_FindCenterMass(std::vector<vec> &vertices, vec &cm);
void Task_2_FindAABB(std::vector<vec> &vertices, vvr::Box3D &aabb);
void Task_3_AlignOriginTo(std::vector<vec> &vertices, const vec &cm);
void Task_4_Draw_PCA(vec &center, vec &dir);
void Task_5_Intersect(std::vector<vvr::Triangle>& triangles, Plane &plane, std::vector<int> &intersection_indices);
void Task_5_Split(vvr::Mesh &mesh, Plane &plane);
void pca(std::vector<vec>& vertices, vec &center, vec &dir);
void FindSubMeshes(vvr::Mesh &mesh, vvr::Canvas2D &canvas);

class Mesh3DScene : public vvr::Scene
{
    enum {
        SHOW_AXES, SHOW_WIRE, SHOW_SOLID, SHOW_NORMALS, SHOW_PLANE, SHOW_AABB, SHOW_PCA,
        SHOW_INTERSECTIONS, SHOW_CM, SPLIT_INSTEAD_OF_INTERSECT, MODE_PARTITION,
    };

public:
    Mesh3DScene();
    const char* getName() const { return "3D Scene"; }
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
    vvr::Canvas2D m_canvas;
    vvr::Colour m_obj_col;
    vvr::Mesh::Ptr m_model_original, m_model;
    vvr::Box3D m_aabb;
    math::vec m_center_mass;
    math::vec m_pca_cen;
    math::vec m_pca_dir;
    math::Plane m_plane;
    std::vector<int> m_intersections;
};
