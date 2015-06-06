#include "GeoLab_02_IntersectionsScene.h"
#include <utils.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <algorithm>

/* Construct - Load  - Setup */

IntersectionsScene::IntersectionsScene()
{
    m_bg_col = Colour(0x44, 0x44, 0x44);
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
    m_canvas_0.add(m_bound_horizontal, Colour::black);
    m_canvas_0.add(m_bound_vertical, Colour::black);

    // Setup Task 1:
    {
        C2DPoint a1(-300, 100);
        C2DPoint a2(-100, 200);
        C2DPoint b1(-350, 230);
        C2DPoint b2(-50, 50);

        m_line_1 = C2DLine(a1, a2);
        m_line_2 = C2DLine(b1, b2);
        m_canvas_0.add(a1, Colour::orange);
        m_canvas_0.add(a2, Colour::orange);
        m_canvas_0.add(m_line_1, Colour::orange);
        m_canvas_1.add(b1, Colour::cyan);
        m_canvas_1.add(b2, Colour::cyan);
        m_canvas_1.add(m_line_2, Colour::cyan);
    }

    // Setup Task 2:
    {
        C2DPoint c1(166, 112);
        C2DPoint c2(290, 150);

        m_circle_1 = C2DCircle(c1, 80);
        m_circle_2 = C2DCircle(c2, 60);
        m_canvas_0.add(c1, Colour::orange);
        m_canvas_0.add(m_circle_1, Colour::orange);
        m_canvas_2.add(c2, Colour::cyan);
        m_canvas_2.add(m_circle_2, Colour::cyan);
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

        m_canvas_0.add(m_triangle_1, Colour::orange);
        m_canvas_3.add(m_triangle_2, Colour::cyan);
    }

    Task1(m_line_2.GetPointTo());
    Task2(m_circle_2.GetCentre());
    Task3(m_triangle_2.GetPoint3());
}

/* UI Handling */

void IntersectionsScene::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);
    echo(x);
    echo(y);
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
    m_canvas_1.add(p, Colour::cyan);
    m_canvas_1.add(p1, Colour::cyan);
    m_canvas_1.add(m_line_2, Colour::cyan);

    /**
    * Breite to simeio tomis twn 2 euth. tmimatwn
    */

    bool seg_intersect = false;
    C2DPoint i;

    //...
    //...
    //...

    m_canvas_1.add(i, seg_intersect ? Colour::green : Colour::red);
}

void IntersectionsScene::Task2(const C2DPoint &p)
{
    m_circle_2.SetCentre(p);
    m_canvas_2.clear();
    m_canvas_2.add(p, Colour::cyan);
    m_canvas_2.add(m_circle_2, Colour::cyan);

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

    //...
    //...
    //...

    m_canvas_2.add(i1, Colour::red);
    m_canvas_2.add(i2, Colour::red);
}

void IntersectionsScene::Task3(const C2DPoint &p)
{
    const C2DPoint &p1 = m_triangle_2.GetPoint1();
    const C2DPoint &p2 = m_triangle_2.GetPoint2();
    m_triangle_2.Set(p1, p2, p);
    m_canvas_3.clear();
    m_canvas_3.add(m_triangle_2, Colour::cyan);

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

    returnFromPixelMode();
}

/* Application Entry Point */

int main(int argc, char* argv[])
{
    return vvr::mainLoop(argc, argv, new IntersectionsScene);
}
