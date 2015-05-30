#include "GeoLab_01_Scene_ConvexHull.h"
#include <utils.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <algorithm>

#define MAX_NUM_PTS 50
#define ConvexHull ConvexHull_Slow // "_Slow"  =>  "_Fast"  for reference implementation

/* Constructor - Loading */

Scene_ConvexHull::Scene_ConvexHull()
{
    m_bg_col = Colour(0x44, 0x44, 0x44);
    reset();
}

/* UI handling */

void Scene_ConvexHull::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);

    for (int i = 0; i < m_convex_hull_polygon.GetPointsCount(); i++) {
        m_convex_hull_polygon.GetPoint(i)->x;
        m_convex_hull_polygon.GetPoint(i)->y;

        m_convex_hull_polygon.GetPoint(i + 1)->x;
        m_convex_hull_polygon.GetPoint(i + 1)->y;


    }

}

void Scene_ConvexHull::mouseMoved(int x, int y, int modif)
{
    Scene::mouseMoved(x, y, modif);
    mousePressed(x, y, modif);
}

void Scene_ConvexHull::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
}

void Scene_ConvexHull::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::LEFT) {

    }
    else if (dir == vvr::RIGHT) {

    }

}

void Scene_ConvexHull::reset()
{
    Scene::reset();
    m_canvas.clear();
    m_convex_hull_polygon.Clear();
    const int BW = 200; 
    const int BH = 200;
    C2DRect bound(-BW, BH, BW, -BH);
    C2DPolygon rand_poly;
    rand_poly.CreateRandom(bound, MAX_NUM_PTS, MAX_NUM_PTS);
    m_point_cloud.DeleteAll();
    rand_poly.GetPointsCopy(m_point_cloud);

    cout << "Computing Convex Hull..." << endl;
    float duration = getSeconds();
    ConvexHull();
    duration = getSeconds() - duration;
    cout << "Duration: " << duration << " sec" << endl;
}

/* Drawing */

void Scene_ConvexHull::draw()
{
    enterPixelMode();
    
    // Do any drawing here.
    draw(m_convex_hull_polygon, Colour::green);
    draw(m_point_cloud, Colour::black);
}

void Scene_ConvexHull::draw(C2DPointSet &point_set, Colour &col)
{
    /* Draw point cloud */
    for (int i = 0; i < point_set.size(); i++) {
        Point2D(
            point_set.GetAt(i)->x,
            point_set.GetAt(i)->y,
            col).draw();
    }
}

void Scene_ConvexHull::draw(C2DLineSet  &line_set, Colour &col)
{
    for (int i = 0; i < line_set.size(); i++) {
        LineSeg2D(
            line_set.GetAt(i)->GetPointFrom().x,
            line_set.GetAt(i)->GetPointFrom().y,
            line_set.GetAt(i)->GetPointTo().x,
            line_set.GetAt(i)->GetPointTo().y,
            col).draw();
    }
}

void Scene_ConvexHull::draw(C2DPolygon  &polygon, Colour &col)
{
    for (int i = 0; i < polygon.GetPointsCount(); i++) {
        LineSeg2D(
            polygon.GetPoint(i)->x,
            polygon.GetPoint(i)->y,
            polygon.GetPoint(i + 1)->x,
            polygon.GetPoint(i + 1)->y,
            col).draw();

        Point2D(
            polygon.GetPoint(i)->x,
            polygon.GetPoint(i)->y,
            col).draw();
    }
}

/* Convex Hull - Implementations */

void Scene_ConvexHull::ConvexHull_Slow()
{
    // HINT:
    // An `p` einai C2DPoint* tote gia na parete to `N` simeio tou point cloud 
    // grafete to eksis:
    //     p = m_point_cloud.GetAt(N);
    // 
    // Dimiourgia euthigrammou tmimatos apo 2 simeia:
    //     C2DLine line( p1,  p2);
    // 
    // To idio, alla me pointers sta simeia:
    //     C2DLine line(*p1, *p2); 
    // 
    // Stin metavliti `lineset` prepei na apothikeysete ta eythigramma tmimata
    // pou vriskete oti anikoun sto Convex Hull.
    //
    // An `pq` einai ena C2DLine, etsi to prosthetete sto lineset:
    //     lineset.AddCopy(pq);
    //

    const int num_pts = m_point_cloud.size(); // To plithos twn simeiwn.
    C2DPoint *p, *q, *t;
    C2DLineSet lineset;

    for (int pi = 0; pi < num_pts; pi++) {
        for (int qi = 0; qi < num_pts; qi++) {
            for (int ti = 0; ti < num_pts; ti++) {
                //...
                //...
                //...
            }
        }
    }

    // Convert line-set to polygon
    C2DPoint *pts = new C2DPoint[lineset.size() * 2];
    for (int i = 0; i < lineset.size(); i++)
        pts[i] = lineset.GetAt(i)->GetPointFrom();
    m_convex_hull_polygon.Create(pts, lineset.size(), true);
    delete[] pts;
}

void Scene_ConvexHull::ConvexHull_Fast()
{
    // Copy the points to a temporary array,
    // to be compatible with GeoLib's functions.
    C2DPoint *pts = new C2DPoint[m_point_cloud.size()];
    for (int i = 0; i < m_point_cloud.size(); i++)
        pts[i] = *m_point_cloud.GetAt(i);

    // Create polygon from points/
    C2DPolygon cloud_polygon;
    cloud_polygon.Create(pts, m_point_cloud.size());

    // Create Convex Hull
    m_convex_hull_polygon.CreateConvexHull(cloud_polygon);

    // Don't forget to delete the array.
    delete[] pts;
}

/* Application entry point */

int main(int argc, char* argv[])
{
    return vvr::mainLoop(argc, argv, new Scene_ConvexHull);
}
