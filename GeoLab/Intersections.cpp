#ifndef SCENE_INTERSECTIONS_H
#define SCENE_INTERSECTIONS_H

#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vvr/utils.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <algorithm>
#include <GeoLib.h>

class IntersectionsScene : public vvr::Scene
{
public:
    IntersectionsScene();

    const char* getName() const override {
        return "UNIVERSITY OF PATRAS - VVR GROUP - COMPUTATIONAL GEOMETRY LAB";
    }

protected: // Overriden methods
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;

private: // Methods
    void Task1(const C2DPoint &p);
    void Task2(const C2DPoint &p);
    void Task3(const C2DPoint &p);

private: // Data
    C2DLine         m_bound_horizontal;
    C2DLine         m_bound_vertical;
    vvr::Canvas   m_canvas_0;
    vvr::Canvas   m_canvas_1;
    vvr::Canvas   m_canvas_2;
    vvr::Canvas   m_canvas_3;
    vvr::Canvas   m_canvas_4;
    C2DTriangle     m_triangle_1;
    C2DTriangle     m_triangle_2;
    C2DCircle       m_circle_1;
    C2DCircle       m_circle_2;
    C2DLine         m_line_1;
    C2DLine         m_line_2;
};

#endif // SCENE_INTERSECTIONS_H
#include <vvr/utils.h>
#include <vvr/macros.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <algorithm>

using namespace std;
using namespace vvr;

/* Construct - Load  - Setup */

IntersectionsScene::IntersectionsScene()
{
    m_bg_col = vvr::grey;
    reset();
}

void IntersectionsScene::reset()
{
    Scene::reset();

    // Clear everything
    m_canvas_0.clear();
    m_canvas_1.clear();
    m_canvas_2.clear();
    m_canvas_3.clear();

    // Divide window to Tasks
    m_bound_vertical.Set(C2DPoint(0, -3000), C2DPoint(0, 3000));
    m_bound_horizontal.Set(C2DPoint(4000, 0), C2DPoint(-4000, 0));
    m_canvas_0.add(m_bound_horizontal, vvr::black);
    m_canvas_0.add(m_bound_vertical, vvr::black);

    // Setup Task 1:
    {
        C2DPoint a1(-300, 100);
        C2DPoint a2(-100, 200);
        C2DPoint b1(-350, 230);
        C2DPoint b2(-50, 50);

        m_line_1 = C2DLine(a1, a2);
        m_line_2 = C2DLine(b1, b2);
        m_canvas_0.add(a1, vvr::orange);
        m_canvas_0.add(a2, vvr::orange);
        m_canvas_0.add(m_line_1, vvr::orange);
        m_canvas_1.add(b1, vvr::cyan);
        m_canvas_1.add(b2, vvr::cyan);
        m_canvas_1.add(m_line_2, vvr::cyan);
    }

    // Setup Task 2:
    {
        C2DPoint c1(166, 112);
        C2DPoint c2(290, 150);

        m_circle_1 = C2DCircle(c1, 80);
        m_circle_2 = C2DCircle(c2, 60);
        m_canvas_0.add(c1, vvr::orange);
        m_canvas_0.add(m_circle_1, vvr::orange);
        m_canvas_2.add(c2, vvr::cyan);
        m_canvas_2.add(m_circle_2, vvr::cyan);
    }

    // Task 3:
    {
        C2DPoint t1a(-300, -50);
        C2DPoint t1b(-40, -45);
        C2DPoint t1c(-70, -170);
        m_triangle_1 = C2DTriangle(t1a, t1b, t1c);

        C2DPoint t2a(-197, -266);
        C2DPoint t2b(-368, -136);
        C2DPoint t2c(-108, -76);
        m_triangle_2 = C2DTriangle(t2a, t2b, t2c);

        m_canvas_0.add(m_triangle_1, vvr::orange);
        m_canvas_3.add(m_triangle_2, vvr::cyan);
    }

    Task1(m_line_2.GetPointTo());
    Task2(m_circle_2.GetCentre());
    Task3(m_triangle_2.GetPoint3());
}

/* UI Handling */

void IntersectionsScene::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);
    vvr_echo(x);
    vvr_echo(y);
    const C2DPoint p(x, y);
    if (m_bound_horizontal.IsOnRight(p) && !m_bound_vertical.IsOnRight(p)) Task1(p);
    if (m_bound_horizontal.IsOnRight(p) && m_bound_vertical.IsOnRight(p)) Task2(p);
    if (!m_bound_horizontal.IsOnRight(p) && !m_bound_vertical.IsOnRight(p)) Task3(p);
    //if (!m_bound_horizontal.IsOnRight(p) &&  m_bound_vertical.IsOnRight(p)) Task4(p);
}

void IntersectionsScene::mouseMoved(int x, int y, int modif)
{
    Scene::mouseMoved(x, y, modif);
    mousePressed(x, y, modif);
}

/* Tasks */

void IntersectionsScene::Task1(const C2DPoint &p)
{
    C2DPoint p1 = m_line_2.GetPointFrom();   // To arxiko simeio paremenei idio.
    m_line_2 = C2DLine(p1, p);   // To teliko simeio tis grammis akolouthei to mouse.

    m_canvas_1.clear();
    m_canvas_1.add(p, vvr::cyan);
    m_canvas_1.add(p1, vvr::cyan);
    m_canvas_1.add(m_line_2, vvr::cyan);

    /**
    * Breite to simeio tomis twn 2 euth. tmimatwn
    */

    bool seg_intersect = false;
    C2DPoint i;

//#define ALTSOL

#ifdef ALTSOL
    //!>>> SOLUTION 1
    double l1_x1 = m_line_1.GetPointFrom().x;
    double l1_y1 = m_line_1.GetPointFrom().y;
    double l1_x2 = m_line_1.GetPointTo().x;
    double l1_y2 = m_line_1.GetPointTo().y;
    double l2_x1 = m_line_2.GetPointFrom().x;
    double l2_y1 = m_line_2.GetPointFrom().y;
    double l2_x2 = m_line_2.GetPointTo().x;
    double l2_y2 = m_line_2.GetPointTo().y;

    double l1_a = (l1_y2 - l1_y1) / (l1_x2 - l1_x1);
    double l2_a = (l2_y2 - l2_y1) / (l2_x2 - l2_x1);
    double l1_b = l1_y1 - l1_a * l1_x1;
    double l2_b = l2_y1 - l2_a * l2_x1;

    i.x = (l2_b - l1_b) / (l1_a - l2_a);
    i.y = l1_a * i.x + l1_b;

    seg_intersect = 
        i.x <= max(l1_x1, l1_x2) && 
        i.x >= min(l1_x1, l1_x2) &&
        i.x <= max(l2_x1, l2_x2) &&
        i.x >= min(l2_x1, l2_x2);
    //! SOLUTION 1 <<<

#else

    //!>>> SOLUTION 2
    C2DPoint l1_p1 = m_line_1.GetPointFrom();
    C2DPoint l1_p2 = m_line_1.GetPointTo();;
    C2DPoint l2_p1 = m_line_2.GetPointFrom();
    C2DPoint l2_p2 = m_line_2.GetPointTo();;

    float l1_p1x = l1_p1.x;
    float l1_p1y = l1_p1.y;
    float l1_p2x = l1_p2.x;
    float l1_p2y = l1_p2.y;
    float l2_p1x = l2_p1.x;
    float l2_p1y = l2_p1.y;
    float l2_p2x = l2_p2.x;
    float l2_p2y = l2_p2.y;

    float l1_a = (l1_p2y - l1_p1y) / (l1_p2x - l1_p1x);
    float l2_a = (l2_p2y - l2_p1y) / (l2_p2x - l2_p1x);
    float l1_b = l1_p1y - l1_a * l1_p1x;
    float l2_b = l2_p1y - l2_a * l2_p1x;

    float ix = (l2_b - l1_b) / (l1_a - l2_a);
    float iy = l1_a * ix + l1_b;

    i.Set(ix, iy);

    C2DVector D1(l1_p2 - l1_p1);
    C2DVector d1(i - l1_p1);
    C2DVector D2(l2_p2 - l2_p1);
    C2DVector d2(i - l2_p1);

    float t1 = d1.Dot(D1);
    float t2 = d2.Dot(D2);

    seg_intersect =
        t1 >= 0 &&
        t2 >= 0 &&
        t1 <= vvr_square(m_line_1.GetLength()) &&
        t2 <= vvr_square(m_line_2.GetLength());
    //! SOLUTION 2 <<<
#endif

    m_canvas_1.add(i, seg_intersect ? vvr::green : vvr::red);
}

void IntersectionsScene::Task2(const C2DPoint &p)
{
    m_circle_2.SetCentre(p);
    m_canvas_2.clear();
    m_canvas_2.add(p, vvr::cyan);
    m_canvas_2.add(m_circle_2, vvr::cyan);

    const double x1 = m_circle_1.GetCentre().x;
    const double y1 = m_circle_1.GetCentre().y;
    const double r1 = m_circle_1.GetRadius();
    const double x2 = m_circle_2.GetCentre().x;
    const double y2 = m_circle_2.GetCentre().y;
    const double r2 = m_circle_2.GetRadius();

    /**
    * Breite ta simeia tomis twn 2 kyklwn.
    */

    C2DPoint i1, i2;

    //---SOLUTION---
    double d = sqrt(vvr_square(x1 - x2) + vvr_square(y1 - y2));
    double l = (vvr_square(r1) - vvr_square(r2) + vvr_square(d)) / (d * 2);
    double h = sqrt(vvr_square(r1) - vvr_square(l));
    double i1x = (x2 - x1)*l / d + (y2 - y1)*h / d + x1;
    double i1y = (y2 - y1)*l / d - (x2 - x1)*h / d + y1;
    i1 = C2DPoint(i1x, i1y);
    double i2x = (x2 - x1)*l / d - (y2 - y1)*h / d + x1;
    double i2y = (y2 - y1)*l / d + (x2 - x1)*h / d + y1;
    i2 = C2DPoint(i2x, i2y);
    //---SOLUTION---

    m_canvas_2.add(i1, vvr::red);
    m_canvas_2.add(i2, vvr::red);
}

void IntersectionsScene::Task3(const C2DPoint &p)
{
    const C2DPoint &p1 = m_triangle_2.GetPoint1();
    const C2DPoint &p2 = m_triangle_2.GetPoint2();
    m_triangle_2.Set(p1, p2, p);
    m_canvas_3.clear();
    m_canvas_3.add(m_triangle_2, vvr::cyan);

    /**
    * Breite ola ta simeia tomis twn 2 trigwnwn
    * kai kante ta add sto m_canvas_3.
    */

    //...
    //...
    //...
}

/* Drawing */

void IntersectionsScene::draw()
{
    enterPixelMode();

    m_canvas_0.draw();
    m_canvas_1.draw();
    m_canvas_2.draw();
    m_canvas_3.draw();

    exitPixelMode();
}

/* Application Entry Point */

int main(int argc, char* argv[])
{
    try
    {
        return vvr::main_with_scene(argc, argv, new IntersectionsScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
        return 1;
    }
}
