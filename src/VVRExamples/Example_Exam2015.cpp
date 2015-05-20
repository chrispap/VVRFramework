#define  NOMINMAX // Fixes a problem on windows
#include "Example_Exam2015.h"
#include <utils.h>
#include <canvas.h>
#include <iostream>
#include <MathGeoLib/MathGeoLib.h>

#define FLAG_SHOW_AXES       1
#define FLAG_SHOW_AABB       2
#define FLAG_SHOW_WIRE       4
#define FLAG_SHOW_SOLID      8
#define FLAG_SHOW_NORMALS   16

GeoLabExam2015Scene::GeoLabExam2015Scene()
{
    m_perspective_proj = false;
    m_globRot_def = Vec3d();
    m_style_flag = FLAG_SHOW_SOLID;
    m_obj_col = Colour(0x45, 0x45, 0x45);
    m_globRot = m_globRot_def;
    m_bg_col = Colour(0x76, 0x8E, 0x77);

    // Load 3D models.
    const string objDir = getBasePath() + "resources/obj/";
    const string objFile = getBasePath() + "resources/obj/bunny.obj";
    m_mesh = Mesh::Make(objDir, objFile, "", true);

    // Setup
    path_width = 80;
    A = C2DPoint(-270, 118);
    B = C2DPoint(90, 250);
    C = C2DPoint(392, 110);
    boundary1.Set(C2DPoint(4000, 0), C2DPoint(-4000, 0));

    SetupTask3();
}

void GeoLabExam2015Scene::resize()
{
    static bool FIRST_PASS = true;

    if (FIRST_PASS)
    {
        m_mesh->setBigSize(getSceneWidth() / 5);
        m_mesh->centerAlign();
        m_mesh->setPos(Vec3d(-getSceneWidth() / 4, -getSceneHeight() / 4, 0));
        m_mesh->update();
    }

    FIRST_PASS = false;
}

void GeoLabExam2015Scene::SetupTask3()
{
    // Make line segments
    C2DLine AB(A, B);
    C2DLine BC(B, C);

    // Make path edges
    C2DVector vw1(A, B);
    vw1.SetLength(path_width);
    vw1.TurnRight();
    C2DVector vw2(B, C);
    vw2.SetLength(path_width);
    vw2.TurnRight();
    C2DLine E2(A + vw1, B + vw1);
    C2DLine E1(A + vw1*-1, B + vw1*-1);
    C2DLine E4(B + vw2, C + vw2);
    C2DLine E3(B + vw2*-1, C + vw2*-1);
    E1.GrowFromCentre(100);
    E2.GrowFromCentre(100);
    E3.GrowFromCentre(100);
    E4.GrowFromCentre(100);

    // Find edge intersections
    C2DPointSet S;
    E1.Crosses(boundary1, &S);
    E2.Crosses(boundary1, &S);
    E1.Crosses(E3, &S);
    E2.Crosses(E4, &S);
    E4.Crosses(boundary1, &S);
    E3.Crosses(boundary1, &S);
    S[0] = A + vw1*-1;
    S[1] = A + vw1;
    S[4] = C + vw2;
    S[5] = C + vw2*-1;

    // Make dashed line helper vars
    C2DVector l1(A, B);
    C2DVector g1(l1);
    l1.SetLength(20);
    g1.SetLength(-5);
    C2DVector l2(B, C);
    C2DVector g2 = l2;
    l2.SetLength(22);
    g2.SetLength(-5);

    if (S.size() != 6) return;

    /* Add shapes */
    canvas3.clear();
    canvas3.add(C2DLine(S[0], S[2]), Colour::white);
    canvas3.add(C2DLine(S[1], S[3]), Colour::white);
    canvas3.add(C2DLine(S[2], S[5]), Colour::white);
    canvas3.add(C2DLine(S[3], S[4]), Colour::white);
    canvas3.add(C2DCircle(A, path_width), Colour::white, 0);
    canvas3.add(C2DCircle(C, path_width), Colour::white, 0);
    canvas3.add(C2DTriangle(S[0], S[2], S[1]), Colour::grey, 1);
    canvas3.add(C2DTriangle(S[1], S[2], S[3]), Colour::grey, 1);
    canvas3.add(C2DTriangle(S[2], S[5], S[4]), Colour::grey, 1);
    canvas3.add(C2DTriangle(S[3], S[2], S[4]), Colour::grey, 1);
    canvas3.add(C2DCircle(A, path_width), Colour::grey, true);
    canvas3.add(C2DCircle(C, path_width), Colour::grey, true);
    for (int i = 0; i < 19; i++) canvas3.add(C2DLine(A + l1*i, A + l1*(i + 1) + g1), Colour::white);
    for (int i = 0; i < 15; i++) canvas3.add(C2DLine(B + l2*i, B + l2*(i + 1) + g2), Colour::white);
    canvas3.add(C2DLine(AB.GetMidPoint(), AB.GetMidPoint() + vw1), Colour::blue);

    canvas3.add(A, Colour::cyan);
    canvas3.add(C, Colour::cyan);
    canvas3.add(B, Colour::cyan);
}

void GeoLabExam2015Scene::mousePressed(int x, int y, int modif)
{
    m_mouselastX = x;
    m_mouselastY = y;
    mouseMoved(x, y, modif);
}

void GeoLabExam2015Scene::mouseMoved(int x, int y, int modif)
{
    C2DPoint p(x, y);

    if (p.y > 0) {
        HandleTask3(p);
    }
    else if (p.x > 0) {
        HandleTask2(p);
    }
    else {
        HandleTask1(p);

        int dx = x - m_mouselastX;
        int dy = y - m_mouselastY;

        Vec3d rot = m_mesh->getRot();

        rot.x -= dy;
        rot.y += dx;

        rot.x = fmod(rot.x, 360.0);
        while (rot.x < 0) rot.x += 360;

        rot.y = fmod(rot.y, 360.0);
        while (rot.y < 0) rot.y += 360;

        m_mouselastX = x;
        m_mouselastY = y;

        m_mesh->setRot(rot);
    }
}

void GeoLabExam2015Scene::HandleTask1(const C2DPoint &p)
{

}

void GeoLabExam2015Scene::HandleTask2(const C2DPoint &p)
{

}

void GeoLabExam2015Scene::HandleTask3(const C2DPoint &p)
{
    canvas3.resize(1);
    canvas3.newFrame(true);
    canvas3.add(p, Task_3_IsOnPath(A, B, C, p, path_width) ? Colour::green : Colour::red);
}

void GeoLabExam2015Scene::draw()
{
    // Draw mesh
    if (m_style_flag & FLAG_SHOW_SOLID)     m_mesh->draw(m_obj_col, SOLID);
    if (m_style_flag & FLAG_SHOW_WIRE)      m_mesh->draw(Colour::black, WIRE);
    if (m_style_flag & FLAG_SHOW_NORMALS)   m_mesh->draw(Colour::black, NORMALS);
    if (m_style_flag & FLAG_SHOW_AXES)      m_mesh->draw(Colour::black, AXES);
    if (m_style_flag & FLAG_SHOW_AABB)      m_mesh->draw(Colour::black, BOUND);


    enterPixelMode();

    // Draw Task 1
    canvas3.draw();

    // Split screen
    Line2D(0, 0, 1, 0).draw();
    LineSeg2D(0, 0, 0, -100000).draw();

    returnFromPixelMode();
}

void GeoLabExam2015Scene::keyEvent(unsigned char key, bool up, int modif)
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

/*! Dosmenou enos dromou pou orizetai apo ta euthigrama tmhmata(?) AB,BC, kai me platos
*  path_width apo tis eutheies, opws fainetai sto sxhma kai enos shmeiou p ypologiste
*  kai epistrepste true h false an to shmeio afto brisketai entos h ektos tou dromou.
* @param[in]  A To shmeio A tou dromou opws fainetai sto sxhma
* @param[in]  B To shmeio B tou dromou opws fainetai sto sxhma
* @param[in]  C To shmeio C tou dromou opws fainetai sto sxhma
* @param[in]  p To shmeio pou kaneis click me to pontiki
* @param[in]  path_width To paxos tou dromou opws fainetai me thn mple gramh sto sxhma
* @return Epistrefei true an to shmeio p einai mesa entos tou dromou alliws false
*/
bool Task_3_IsOnPath(const C2DPoint &A, const C2DPoint &B, const C2DPoint &C, const C2DPoint &p, double path_width)
{
    // Make line segments
    C2DLine AB(B, A);
    C2DLine BC(B, C);

    double d1, d2, d1_, d2_;
    C2DPoint np1, np2;
    d1 = AB.Distance(p);
    d2 = BC.Distance(p);
    d1_ = AB.DistanceAsRay(p);
    d2_ = BC.DistanceAsRay(p);

    if (d1 == d2) return (d1_ < path_width && d2_ < path_width);
    else return (min(d1, d2) < path_width);
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new GeoLabExam2015Scene);
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}
