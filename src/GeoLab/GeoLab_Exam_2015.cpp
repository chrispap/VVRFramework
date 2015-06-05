#include "GeoLab_Exam_2015.h"
#include <utils.h>
#include <canvas.h>
#include <iostream>
#include <MathGeoLib.h>

#define FLAG_SHOW_AXES       1
#define FLAG_SHOW_AABB       2
#define FLAG_SHOW_WIRE       4
#define FLAG_SHOW_SOLID      8
#define FLAG_SHOW_NORMALS   16

//! Helpers

void CreateRandomPoints(C2DPointSet &ptset, int ptnum, int W0, int W1, int H0, int H1)
{
    ptset.DeleteAll();

    CRandomNumber rnw, rnh;
    rnw.SetMin(W0);
    rnw.SetMax(W1);
    rnh.SetMin(H0);
    rnh.SetMax(H1);

    for (int i = 0; i < ptnum; i++) {
        ptset.AddCopy(rnw.GetInt(), rnh.GetInt());
    }

    // Discard duplicate points
    // After sorting, duplicate points will be consecutive.

    ptset.SortLeftToRight();
    for (int i = 0; i < ptset.size() - 1; i++) {
        if (ptset.GetAt(i)->Distance(*ptset.GetAt(i + 1)) == 0.0) {
            ptset.DeleteAt(i--);
        }
    }
}

void ConvexHull(const C2DPointSet &pset_in, C2DPointSet &pset_hull)
{
    C2DPointSet tmp_set;
    tmp_set.AddCopy(pset_in);
    pset_hull.ExtractConvexHull(tmp_set);
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new Exam2015Scene);
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}

//! Scene Class

Exam2015Scene::Exam2015Scene()
{
    // Setup [Scene Params]
    {
        m_perspective_proj = false;
        m_globRot_def = Vec3d(10, 10, 0);
        m_style_flag = FLAG_SHOW_SOLID | FLAG_SHOW_AABB;
        m_obj_col = Colour(0x45, 0x45, 0x45);
        m_globRot = m_globRot_def;
        m_bg_col = Colour(0x76, 0x8E, 0x77);
    }

    // Setup Task [Lines]
    {
        CreateRandomPoints(m_point_set, 18, 80, 300, -80, -300);
        ConvexHull(m_point_set, m_convex_hull);
        m_line1.Set(C2DPoint(15, -5), C2DPoint(15, -400));
        m_line2.Set(C2DPoint(410, -5), C2DPoint(410, -400));
    }

    // Setup Task [Path]
    {
        m_path_width = 80;
        A = C2DPoint(-270, 118);
        B = C2DPoint(90, 250);
        C = C2DPoint(392, 110);
        C2DLine boundary1(C2DPoint(4000, 0), C2DPoint(-4000, 0));

        // Make line segments
        C2DLine AB(A, B);
        C2DLine BC(B, C);

        // Make path edges
        C2DVector vw1(A, B);
        vw1.SetLength(m_path_width);
        vw1.TurnRight();
        C2DVector vw2(B, C);
        vw2.SetLength(m_path_width);
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

        if (S.size() != 6)
            return;

        // Add shapes
        m_path_canvas.clear();
        m_path_canvas.add(C2DLine(S[0], S[2]), Colour::white);
        m_path_canvas.add(C2DLine(S[1], S[3]), Colour::white);
        m_path_canvas.add(C2DLine(S[2], S[5]), Colour::white);
        m_path_canvas.add(C2DLine(S[3], S[4]), Colour::white);
        m_path_canvas.add(C2DCircle(A, m_path_width), Colour::white, 0);
        m_path_canvas.add(C2DCircle(C, m_path_width), Colour::white, 0);
        m_path_canvas.add(C2DTriangle(S[0], S[2], S[1]), Colour::grey, 1);
        m_path_canvas.add(C2DTriangle(S[1], S[2], S[3]), Colour::grey, 1);
        m_path_canvas.add(C2DTriangle(S[2], S[5], S[4]), Colour::grey, 1);
        m_path_canvas.add(C2DTriangle(S[3], S[2], S[4]), Colour::grey, 1);
        m_path_canvas.add(C2DCircle(A, m_path_width), Colour::grey, true);
        m_path_canvas.add(C2DCircle(C, m_path_width), Colour::grey, true);

        for (int i = 0; i < 19; i++)
            m_path_canvas.add(C2DLine(A + l1*i, A + l1*(i + 1) + g1), Colour::white);

        for (int i = 0; i < 15; i++)
            m_path_canvas.add(C2DLine(B + l2*i, B + l2*(i + 1) + g2), Colour::white);

        m_path_canvas.add(C2DLine(AB.GetMidPoint(), AB.GetMidPoint() + vw1), Colour::blue);
        m_path_canvas.add(A, Colour::cyan);
        m_path_canvas.add(C, Colour::cyan);
        m_path_canvas.add(B, Colour::cyan);
    }

    // Setup Task [3D]
    {
        const string objDir = getBasePath() + "resources/obj/";
        const string objFile = getBasePath() + "resources/obj/dolphin.obj";
        m_mesh = Mesh(objDir, objFile, "", true);
    }

    // Execute Tasks
    Task_Parallel_Lines();
    Task_3D();
}

void Exam2015Scene::resize()
{
    static bool FIRST_PASS = true;

    if (FIRST_PASS)
    {
        m_mesh.setBigSize(getSceneWidth() / 5);
        m_mesh.centerAlign();
        m_mesh.setPos(Vec3d(-getSceneWidth() / 4, -getSceneHeight() / 4, 0));
        m_mesh.update();
    }

    FIRST_PASS = false;
}

void Exam2015Scene::mousePressed(int x, int y, int modif)
{
    m_mouse_last_x = x;
    m_mouse_last_y = y;
    mouseMoved(x, y, modif);
}

void Exam2015Scene::mouseMoved(int x, int y, int modif)
{
    C2DPoint p(x, y);

    // Task 3
    if (p.y > 0)
    {
        m_path_canvas.resize(1);
        m_path_canvas.newFrame(true);
        m_path_canvas.add(p, Task_Path(p) ? Colour::green : Colour::red);
    }

    // Task 2
    else if (p.x > 0)
    {

    }

    // Task 1
    else
    {

        int dx = x - m_mouse_last_x;
        int dy = y - m_mouse_last_y;

        Vec3d rot = m_mesh.getRot();

        rot.x -= dy;
        rot.y += dx;

        rot.x = fmod(rot.x, 360.0);
        while (rot.x < 0) rot.x += 360;

        rot.y = fmod(rot.y, 360.0);
        while (rot.y < 0) rot.y += 360;

        m_mouse_last_x = x;
        m_mouse_last_y = y;

        m_mesh.setRot(rot);
    }
}

void Exam2015Scene::keyEvent(unsigned char key, bool up, int modif)
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

void Exam2015Scene::draw()
{
    // Draw mesh
    if (m_style_flag & FLAG_SHOW_SOLID)     m_mesh.draw(m_obj_col, SOLID);
    if (m_style_flag & FLAG_SHOW_WIRE)      m_mesh.draw(Colour::black, WIRE);
    if (m_style_flag & FLAG_SHOW_NORMALS)   m_mesh.draw(Colour::black, NORMALS);
    if (m_style_flag & FLAG_SHOW_AXES)      m_mesh.draw(Colour::black, AXES);
    if (m_style_flag & FLAG_SHOW_AABB)      m_mesh.draw(Colour::black, BOUND);

    enterPixelMode();
    m_path_canvas.draw();
    Canvas2D tmp_canvas;
    tmp_canvas.add(m_line1, Colour::yellow);
    tmp_canvas.add(m_line2, Colour::yellowGreen);
    tmp_canvas.draw();
    vvr::draw(m_point_set);
    Line2D(0, 0, 1, 0).draw();
    LineSeg2D(0, 0, 0, -100000).draw();
    returnFromPixelMode();
}

//! Tasks

void Exam2015Scene::Task_Parallel_Lines()
{
    // DESCRIPTION:
    //  Breite tis eutheies elaxistis apostasis pou perikleioun ola ta simeia.
    //
    // DEDOMENA:
    //  C2DPointSet m_point_set;
    //  C2DPointSet m_convex_hull;
    //
    // RESULT:
    //  Tis grammes apothikeystese tis stis:
    //   `m_line1`
    //   `m_line2`
    //
    // YPENTHIMIZOUME:
    //  m_line1.Set(C2DPoint(l1_x1, l1_y1), C2DPoint(l1_x2, l1_y2));
    //  m_line1.Set(C2DPoint(l2_x1, l2_y1), C2DPoint(l2_x2, l2_y2));
    //

    m_line1.Set(m_convex_hull[0], m_convex_hull[1]);

    double dist = std::numeric_limits<double>::max();
    int l1, l2;
    for (int i = 0; i < m_convex_hull.size() - 1; i++)
    {
        double tempDist = std::numeric_limits<double>::min();
        int ltemp;
        for (int j = 0; j < m_convex_hull.size(); j++)
        {
            double temp;
            if (j != i && j != i + 1)
            {
                temp = C2DLine(m_convex_hull[i], m_convex_hull[i + 1]).DistanceAsRay(m_convex_hull[j]);
                if (temp > tempDist)
                {
                    tempDist = temp;
                    ltemp = j;
                }
            }
        }
        if (tempDist < dist)
        {
            dist = tempDist;
            l1 = i;
            l2 = ltemp;
        }
    }

    C2DPoint to2 = m_convex_hull[l2] + m_convex_hull[l1 + 1] - m_convex_hull[l1];

    C2DLine r1(m_convex_hull[l1], m_convex_hull[l1 + 1]);
    C2DLine r2(m_convex_hull[l2], to2);

    r1.Grow(2, r1.GetMidPoint());
    r2.Grow(2, r2.GetMidPoint());

    m_line1 = r1;
    m_line2 = r2;
}

bool Exam2015Scene::Task_Path(const C2DPoint &p)
{
    // DESCRIPTION:
    //  Dosmenou enos dromou pou orizetai apo ta euthigrama tmhmata(?) AB,BC, kai me platos
    //  path_width apo tis eutheies, opws fainetai sto sxhma kai enos shmeiou p ypologiste
    //  kai epistrepste true h false an to shmeio afto brisketai entos h ektos tou dromou.
    //
    // DEDOMENA:
    //  float     m_path_width : To paxos tou dromou. (Mple gramh sto sxhma)
    //  C2DPoint  A, B, C;     : Ta 3 simeia pou orizoun ton dromo. (Left-to-right)
    //  C2DPoint  p            : To shmeio pou kaneis click me to pontiki.
    //
    // RETURN:
    //  Prepei na epistrepsete: [true / false]  ==>  [Entos / Ektos] dromou antistoixa.
    //

    const bool quick_solution = true;

    if (quick_solution)
    {
        C2DLine AB(B, A);
        C2DLine BC(B, C);

        double d1, d2, d1_, d2_;
        C2DPoint np1, np2;
        d1 = AB.Distance(p);
        d2 = BC.Distance(p);
        d1_ = AB.DistanceAsRay(p);
        d2_ = BC.DistanceAsRay(p);

        return (d1 == d2) ? (d1_ < m_path_width && d2_ < m_path_width) :
            (min(d1, d2) < m_path_width);
    }

    C2DVector AB(B - A);
    C2DVector PA(p - A);
    C2DVector PB(p - B);
    C2DVector BC(C - B);

    AB.MakeUnit();
    double pToL1 = AB.Dot(PA);
    C2DVector proj1 = AB*pToL1 + C2DVector(A);

    BC.MakeUnit();
    double pToL2 = BC.Dot(PB);
    C2DVector proj2 = BC*pToL2 + C2DVector(B);

    if (pToL1 < 0)
        proj1 = A;
    if (pToL1 > A.Distance(B))
        proj1 = B;
    if (pToL2 < 0)
        proj2 = B;
    if (pToL2 > A.Distance(B))
        proj2 = C;

    return !(p.Distance(proj2) > m_path_width && p.Distance(proj1) > m_path_width);
}

void Exam2015Scene::Task_3D()
{
    // DESCRIPTION:
    //  Breite to pososto ogkou tou AABB pou kalyptei to 3D montelo `m_mesh`;
    //
    // DEDOMENA:
    //  vertices  : Koryfes montelou
    //  triangles : Trigwnna montelou
    //  aabb      : Axis Aligned Bounding Box montelou
    //
    // RESULTS:
    //  To pososto kalypsis apothikeste to stin:
    //   - `coverage_rate`
    //

    vector<Vec3d>           &vertices = m_mesh.getVertices();
    vector<vvr::Triangle>   &triangles = m_mesh.getTriangles();
    double                  coverage_rate = 100;
    vvr::Box                aabb = m_mesh.getBox();

    // Random dist
    double dist = (aabb.max.x - aabb.min.x) / 10.0;
    int inside = 0, outside = 0;
    Vec3d toRay = aabb.max;
    vec to(toRay.x, toRay.y, toRay.z);

    for (double i = aabb.min.x + dist / 2.0; i < aabb.max.x - dist; i += dist) {
        for (double j = aabb.min.y + dist / 2.0; j < aabb.max.y - dist; j += dist) {
            for (double k = aabb.min.z + dist / 2.0; k < aabb.max.z - dist; k += dist) {
                int count = 0;
                vec from(i, j, k);

                for (int it = 0; it < triangles.size(); it++) {
                    vec myV1(triangles[it].v1().x, triangles[it].v1().y, triangles[it].v1().z);
                    vec myV2(triangles[it].v2().x, triangles[it].v2().y, triangles[it].v2().z);
                    vec myV3(triangles[it].v3().x, triangles[it].v3().y, triangles[it].v3().z);
                    math::Triangle tempTris(myV1, myV2, myV3);
                    if (tempTris.Intersects(math::Ray(from, vec(from - to).Normalized()))) {
                        count++;
                    }
                }

                if (count % 2 == 1)
                    inside++;
                else
                    outside++;
            }
        }
    }

    coverage_rate = 100.0 * inside / (inside + outside);

    cout << "AABB Coverage: " << coverage_rate << "%" << endl;
}
