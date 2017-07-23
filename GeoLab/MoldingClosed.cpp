#ifndef SCENE_MOLDING_H
#define SCENE_MOLDING_H

#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vvr/animation.h>
#include "GeoLib.h"

class MoldingScene : public vvr::Scene
{
public:
    MoldingScene();

    const char* getName() const override {
        return "UNIVERSITY OF PATRAS - VVR GROUP - COMPUTATIONAL GEOMETRY LAB";
    }

protected:
    void draw() override;
    void reset() override;
    bool idle() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;

private:
    void findMoldPieces();

private:
    C2DPoint *m_curr_p, m_click_anchor;
    C2DPolygon m_polygon;
    std::vector<C2DPoint> m_pts;
    vvr::Animation m_anim;
    std::vector<C2DPolygon> m_pieces;
    std::vector<C2DVector> m_dirs;
};

#endif
#include <vvr/utils.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace vvr;

#define MOLD_SIDE_MIN_LEN 70
#define SPEED_PIXELS_PER_SEC 25
#define PIECES_SOLID true
#define AUTOPLAY false

/* Hardcoded Molds */

void HARDCODEDMOLD_01(std::vector<C2DPoint> &pts)
{
    pts.clear();
    pts.push_back(C2DPoint(-363, -65));
    pts.push_back(C2DPoint(-273, -98));
    pts.push_back(C2DPoint(-186, -112));
    pts.push_back(C2DPoint(-106, -111));
    pts.push_back(C2DPoint(-40, -109));
    pts.push_back(C2DPoint(15, -101));
    pts.push_back(C2DPoint(68, -65));
    pts.push_back(C2DPoint(134, -59));
    pts.push_back(C2DPoint(195, -67));
    pts.push_back(C2DPoint(258, -73));
    pts.push_back(C2DPoint(338, -68));
    pts.push_back(C2DPoint(397, -24));
    pts.push_back(C2DPoint(415, 111));
    pts.push_back(C2DPoint(332, 171));
    pts.push_back(C2DPoint(155, 212));
    pts.push_back(C2DPoint(31, 245));
    pts.push_back(C2DPoint(-132, 237));
    pts.push_back(C2DPoint(-265, 226));
    pts.push_back(C2DPoint(-375, 151));
    pts.push_back(C2DPoint(-400, 64));
    pts.push_back(C2DPoint(-398, 21));
    pts.push_back(C2DPoint(-399, -28));
}

void HARDCODEDMOLD_02(std::vector<C2DPoint> &pts)
{
    pts.clear();
    pts.push_back(C2DPoint(-362, 2));
    pts.push_back(C2DPoint(-316, -132));
    pts.push_back(C2DPoint(-186, -112));
    pts.push_back(C2DPoint(-120, -139));
    pts.push_back(C2DPoint(-49, -141));
    pts.push_back(C2DPoint(15, -101));
    pts.push_back(C2DPoint(67, -47));
    pts.push_back(C2DPoint(130, -11));
    pts.push_back(C2DPoint(209, -20));
    pts.push_back(C2DPoint(273, -52));
    pts.push_back(C2DPoint(340, -87));
    pts.push_back(C2DPoint(409, -118));
    pts.push_back(C2DPoint(456, -15));
    pts.push_back(C2DPoint(384, 88));
    pts.push_back(C2DPoint(167, 87));
    pts.push_back(C2DPoint(-1, 231));
    pts.push_back(C2DPoint(-132, 237));
    pts.push_back(C2DPoint(-18, 116));
    pts.push_back(C2DPoint(3, 42));
    pts.push_back(C2DPoint(-44, -9));
    pts.push_back(C2DPoint(-115, 17));
    pts.push_back(C2DPoint(-153, -51));
}

void HARDCODEDMOLD_03(std::vector<C2DPoint> &pts)
{
    pts.clear();
    pts.push_back(C2DPoint(-427, -52));
    pts.push_back(C2DPoint(-221, -182));
    pts.push_back(C2DPoint(-237, 26));
    pts.push_back(C2DPoint(-112, -146));
    pts.push_back(C2DPoint(-40, -109));
    pts.push_back(C2DPoint(-157, 67));
    pts.push_back(C2DPoint(-43, 119));
    pts.push_back(C2DPoint(132, -34));
    pts.push_back(C2DPoint(216, 2));
    pts.push_back(C2DPoint(240, 93));
    pts.push_back(C2DPoint(360, -2));
    pts.push_back(C2DPoint(430, 49));
    pts.push_back(C2DPoint(415, 111));
    pts.push_back(C2DPoint(332, 171));
    pts.push_back(C2DPoint(155, 212));
    pts.push_back(C2DPoint(132, 123));
    pts.push_back(C2DPoint(-47, 232));
    pts.push_back(C2DPoint(-248, 172));
    pts.push_back(C2DPoint(-347, 234));
    pts.push_back(C2DPoint(-423, 190));
    pts.push_back(C2DPoint(-373, 103));
    pts.push_back(C2DPoint(-329, 34));
}

/* Task */

int FullPath(vector<double> &angles, int from, double &dep_start, double &dep_angle)
{
    const int size = angles.size();
    const double pi = 3.14159265359;
    bool badAngle = false;
    double min_angle = 0, max_angle = pi;
    double start_angle = angles[from];
    int current_i = (from + 1) % size;
    while (current_i != from)
    {
        double new_angle = angles[current_i] - start_angle;
        double new_max_angle = max_angle, new_min_angle = min_angle;
        if (new_angle > pi)
            new_angle -= 2 * pi;
        else if (new_angle < -pi)
            new_angle += 2 * pi;
        if (new_angle > min_angle)
            new_min_angle = new_angle;
        else if (new_angle + pi < max_angle)
            new_max_angle = new_angle + pi;

        if (new_max_angle > new_min_angle)
        {
            max_angle = new_max_angle;
            min_angle = new_min_angle;
        }
        else
        {
            break;
        }
        current_i = (current_i + 1) % size;
    }
    dep_start = start_angle + min_angle;
    if (dep_start > pi)
        dep_start -= 2 * pi;
    else if (dep_start < -pi)
        dep_start += 2 * pi;
    dep_angle = max_angle - min_angle;
    return current_i;
}

bool ValidChain(vector<double> &angles, int from, int to, double &dep_start, double &dep_angle)
{
    const int size = angles.size();
    const double pi = 3.14159265359;
    bool badAngle = false;
    double min_angle = 0, max_angle = pi;
    double start_angle = angles[from];
    int current_i = (from + 1) % size;
    while (current_i != to)
    {
        double new_angle = angles[current_i] - start_angle;
        double new_max_angle = max_angle, new_min_angle = min_angle;
        if (new_angle > pi)
            new_angle -= 2 * pi;
        else if (new_angle < -pi)
            new_angle += 2 * pi;
        if (new_angle > min_angle)
            new_min_angle = new_angle;
        else if (new_angle + pi < max_angle)
            new_max_angle = new_angle + pi;

        if (new_max_angle > new_min_angle)
        {
            max_angle = new_max_angle;
            min_angle = new_min_angle;
        }
        else
        {
            return false;
        }
        current_i = (current_i + 1) % size;
    }
    dep_start = start_angle + min_angle;
    if (dep_start > pi)
        dep_start -= 2 * pi;
    else if (dep_start < -pi)
        dep_start += 2 * pi;
    dep_angle = max_angle - min_angle;
    return true;
}

bool CanBeMolded(const C2DPolygon &poly, vector<int> &out_indices, vector<double> &out_angles)
{
    int size = poly.GetPointsCount();
    const double pi = 3.14159265359;
    vector<double> angles;
    double dep_start = 0, dep_angle = 0;
    vector<double> dep_start_angles;
    vector<double> dep_angles;

    for (int i = 0; i < size; i++)
    {
        C2DPoint p1 = *poly.GetPoint(i);
        C2DPoint p2 = *poly.GetPoint((i + 1) % size);
        angles.push_back(atan2(p2.y - p1.y, p2.x - p1.x));
    }

    bool done = false;
    bool exact_fit = false;
    int start_index = 0;
    vector<int> cycles[2];

    for (int i = 0; i < 2; i++)
    {
        int current_i = start_index;
        int distance = 0;
        int last_i;
        dep_start_angles.clear();
        dep_angles.clear();
        do
        {
            cycles[i].push_back(current_i);
            last_i = current_i;
            current_i = FullPath(angles, current_i, dep_start, dep_angle);
            distance += (size + current_i - last_i) % size;
            dep_start_angles.push_back(dep_start);
            dep_angles.push_back(dep_angle);
            if (i == 0)
            {
                C2DLine seg(*poly.GetPoint((size + current_i - 1) % size), *poly.GetPoint(current_i));
                if (!seg.IsOnRight(*poly.GetPoint((current_i + 1) % size)))
                {
                    //non-reflex angle: molding impossible
                    return false;
                }
            }
        } while (distance < size);
        if (current_i - start_index < 2)
        {
            out_indices = cycles[i];
            done = true;
            if (current_i == start_index)
                exact_fit = true;
            break;
        }
        else
            cycles[i].push_back(current_i);
        start_index = current_i;
    }

    if (!done)
    {
        vector<int> cycle;
        for (int i = 1; i < cycles[0].back(); i++)
        {
            int current_i = i;
            int distance = 0;
            int last_i;
            cycle.clear();
            dep_start_angles.clear();
            dep_angles.clear();
            do
            {
                cycle.push_back(current_i);
                last_i = current_i;
                current_i = FullPath(angles, current_i, dep_start, dep_angle);
                distance += (size + current_i - last_i) % size;
                dep_start_angles.push_back(dep_start);
                dep_angles.push_back(dep_angle);
            } while (distance < size);
            if (current_i - i < 2)
            {
                if (current_i == start_index)
                    exact_fit = true;
                out_indices = cycle;
                done = true;
            }
        }
        if (!done)
        {
            out_indices = cycle;
        }
    }

    size = dep_angles.size();

    if (!exact_fit)
    {
        if (out_indices.size() > 2)
        {
            dep_angles.pop_back();
            dep_start_angles.pop_back();
            ValidChain(angles, out_indices.back(), out_indices.front(), dep_start, dep_angle);
            dep_start_angles.push_back(dep_start);
            dep_angles.push_back(dep_angle);
        }
    }

    if (size > 2)
    {
        for (int i = 0; i < size; i++)
        {
            int prev = (size + i - 1) % size;
            double angle = dep_start_angles[i] - dep_start_angles[prev];
            if (angle > pi)
            {
                angle -= 2 * pi;
            }
            if (angle < -pi)
            {
                angle += 2 * pi;
            }
            if (angle > 0)
            {
                angle = dep_start_angles[prev] + dep_angles[prev] + (angle - dep_angles[prev]) / 2;
            }
            else
            {
                angle = dep_start_angles[prev] + (angle + dep_angles[i]) / 2;
            }
            if (angle > pi)
            {
                angle -= 2 * pi;
            }
            if (angle < -pi)
            {
                angle += 2 * pi;
            }
            out_angles.push_back(angle);
        }
    }
    else
    {
        for (int i = 0; i < size; i++)
        {
            double theta = angles[out_indices[i]] - angles[(angles.size() + out_indices[i] - 1) % angles.size()];
            if (theta > pi)
                theta -= 2 * pi;
            if (theta < -pi)
                theta += 2 * pi;

            if (theta < 0)
                theta += pi;
            else
                theta -= pi;

            theta /= 2;

            theta += angles[(angles.size() + out_indices[i] - 1) % angles.size()];
            if (theta > pi)
                theta -= 2 * pi;
            if (theta < -pi)
                theta += 2 * pi;
            out_angles.push_back(theta);
        }
    }

    return true;
}

void Mold(const std::vector<C2DPoint> &pts, std::vector<C2DPolygon> &mold_pieces, std::vector<C2DVector> &piece_dir)
{
    if (pts.size() < 3) return;
    mold_pieces.clear();

    vector<C2DPoint> box_pts;
    box_pts.push_back(C2DPoint(-600, +400));
    box_pts.push_back(C2DPoint(-600, -400));
    box_pts.push_back(C2DPoint(+600, -400));
    box_pts.push_back(C2DPoint(+600, +400));
    C2DPolygon box(&box_pts[0], box_pts.size(), false);

    C2DPolygon poly(&pts[0], pts.size(), false);
    poly.MakeClockwise();
    vector<int> indices;
    vector<double> angles;

    if (CanBeMolded(poly, indices, angles))
    {
        vector<C2DPoint> intersections;
        vector<int> intersecting_box_vertex_i;
        const int num_pieces = indices.size();

        for (int i = 0; i < num_pieces; i++)
        {
            C2DVector dir = C2DVector(cos(angles[i]), sin(angles[i]));
            piece_dir.push_back(dir);
            C2DLine seg(*poly.GetPoint(indices[i]), *poly.GetPoint(indices[i]) + dir * 1200);
            C2DPointSet intersections_pointset;
            for (int j = 0; j < 4; j++)
            {
                if (seg.Crosses(C2DLine(box_pts[j], box_pts[(j + 1) % 4]), &intersections_pointset))
                {
                    intersecting_box_vertex_i.push_back(j);
                    intersections.push_back(*intersections_pointset.GetAt(0));
                    break;
                }
            }
        }

        for (int i = 0; i < num_pieces; i++)
        {
            vector<C2DPoint> mold_piece_pts;

            //push first intersection point
            mold_piece_pts.push_back(intersections[i]);
            int current_index = indices[i];
            mold_piece_pts.push_back(*poly.GetPoint(current_index));
            vector<int>::iterator p;
            do
            {
                current_index = (current_index + 1) % pts.size();
                mold_piece_pts.push_back(*poly.GetPoint(current_index));
                p = find(indices.begin(), indices.end(), current_index);
            } while (p == indices.end());

            mold_piece_pts.push_back(intersections[p - indices.begin()]);

            int num_box_pts = (4 + intersecting_box_vertex_i[i] - intersecting_box_vertex_i[p - indices.begin()]) % 4;
            for (int j = 1; j <= num_box_pts; j++)
            {
                //push box points
                mold_piece_pts.push_back(box_pts[(4 + intersecting_box_vertex_i[p - indices.begin()] + j) % 4]);
            }

            mold_pieces.push_back(C2DPolygon(&mold_piece_pts[0], mold_piece_pts.size(), true));
        }
    }
    else
    {
        cout << "Cannot be molded" << endl;
    }
}

/* Scene */

MoldingScene::MoldingScene()
{
    m_bg_col = Colour::grey;
    m_hide_log = false;
    m_fullscreen = true;
    m_curr_p = NULL;
    HARDCODEDMOLD_01(m_pts);
    m_polygon = C2DPolygon(&m_pts[0], m_pts.size(), false);
    findMoldPieces();
    m_anim.update(AUTOPLAY);
}

void MoldingScene::reset()
{
    Scene::reset();
    m_pts.clear();
    m_pieces.clear();
    m_dirs.clear();
}

void MoldingScene::draw()
{
    enterPixelMode();

    static const vvr::Colour Pallete[3] = {
        vvr::Colour::magenta,
        vvr::Colour::darkGreen,
        vvr::Colour::orange
    };

    if (!m_pts.empty())
    {

        //! Draw mold pieces

        for (int i = 0; i < m_pieces.size(); ++i)
        {
            m_anim.update();
            C2DVector d = m_dirs[i] * m_anim.t * SPEED_PIXELS_PER_SEC;
            C2DPolygon p = m_pieces[i];
            p.Move(d);
            vvr::draw(p, Pallete[i % 3], PIECES_SOLID);
        }

        //! Draw molded object

        Colour col1 = Colour(46, 97, 162);
        vvr::draw(m_polygon, col1, true);
        vvr::draw(m_polygon, Colour::black, false);

        //! Object vertices

        for (int i = 0; i < m_pts.size(); ++i)
        {
            vvr::Point2D(m_pts.at(i).x, m_pts.at(i).y).draw();
        }

        //! Selected vertex

        if (m_curr_p)
        {
            vvr::Point2D(m_curr_p->x, m_curr_p->y, vvr::Colour::magenta).draw();
        }
    }

    returnFromPixelMode();
}

bool MoldingScene::idle()
{
    if (m_anim.paused()) return false;
    m_anim.update();
    return true;
}

void MoldingScene::mousePressed(int x, int y, int modif)
{
    C2DPoint p(x, y);

    if (altDown(modif))
    {
        for (int i = 0; i < (int)m_pts.size(); ++i) {
            if (p.Distance(m_pts[i]) < 10) {
                m_curr_p = &m_pts[i];
                m_polygon = C2DPolygon(&m_pts[0], m_pts.size(), false);
                findMoldPieces();
            }
        }
    }
    else if (ctrlDown(modif))
    {
        m_pts.push_back(p);
        m_polygon = C2DPolygon(&m_pts[0], m_pts.size(), false);
        findMoldPieces();
    }
}

void MoldingScene::mouseReleased(int x, int y, int modif)
{
    m_curr_p = NULL;
}

void MoldingScene::mouseMoved(int x, int y, int modif)
{
    C2DPoint p(x, y);

    if (altDown(modif) && m_curr_p)
    {
        *m_curr_p = p;
        m_polygon = C2DPolygon(&m_pts[0], m_pts.size(), false);
        findMoldPieces();
    }
    else if (ctrlDown(modif))
    {
        if (p.Distance(m_pts.back()) > MOLD_SIDE_MIN_LEN) {
            m_pts.push_back(p);
            m_polygon = C2DPolygon(&m_pts[0], m_pts.size(), false);
            findMoldPieces();
        }
    }
}

void MoldingScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    key = tolower(key);

    switch (key)
    {
    case '1':
        HARDCODEDMOLD_01(m_pts);
        m_polygon = C2DPolygon(&m_pts[0], m_pts.size(), false);
        findMoldPieces(); m_anim.update(AUTOPLAY);
        break;
    case '2':
        HARDCODEDMOLD_02(m_pts);
        m_polygon = C2DPolygon(&m_pts[0], m_pts.size(), false);
        findMoldPieces(); m_anim.update(AUTOPLAY);
        break;
    case '3':
        HARDCODEDMOLD_03(m_pts);
        m_polygon = C2DPolygon(&m_pts[0], m_pts.size(), false);
        findMoldPieces(); m_anim.update(AUTOPLAY);
        break;
    case 's':
        for (int i = 0; i < (int)m_pts.size(); ++i) {
            printf("pts.push_back(C2DPoint(%d, %d));\n", (int)m_pts[i].x, (int)m_pts[i].y);
        }
        break;
    case 'm':
        findMoldPieces();
        break;
    case ' ':
        if (m_anim.paused())
            m_anim.update(true);
        else
            m_anim.pause();
        break;
    }
}

void MoldingScene::findMoldPieces()
{
    if (m_pts.size() < 3) return;
    m_pieces.clear();
    m_dirs.clear();
    Mold(m_pts, m_pieces, m_dirs);
    m_anim.setTime(0);
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new MoldingScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
        return 1;
    }
}
