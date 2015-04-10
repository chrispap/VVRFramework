#include "ContourEditorScene.h"
#include "utils.h"
#include "canvas.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#define CONTOUR_FILENAME  "contours.txt"
#define CONTOUR_DELIMITER "CONTOUR-LINE"

using namespace vvr;
using namespace std;

ContourEditorScene::ContourEditorScene()
{
    m_bg_col = Colour(0x44, 0x44, 0x44);
    m_pts.resize(1);
    b_render_pts = false;
    loadContoursFromFile(getBasePath() + CONTOUR_FILENAME);
}

void ContourEditorScene::draw()
{
    enterPixelMode();

    // Draw all line-strips. (Contours)
    Colour line_col;

    for (int ci=0; ci<m_pts.size(); ci++)
    {
        for (int pi=0; pi<m_pts[ci].size(); pi++)
        {
            Vec3d &p1 = m_pts[ci][pi];
            Vec3d &p2 = m_pts[ci][(pi+1)%m_pts[ci].size()];

            line_col = Colour::yellow;

            if (ci==m_pts.size()-1 && pi==m_pts[ci].size()-1) {
                line_col = Colour::grey;
            }

            LineSeg2D(p1.x, p1.y, p2.x, p2.y, line_col).draw();
            if (b_render_pts)
                Point2D(p1.x, p1.y, Colour::yellow).draw();
        }
    }

}

void ContourEditorScene::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);

    if (modif) {
        m_pts.resize(m_pts.size()+1);
    }

    m_pts.back().push_back(Vec3d(x,y,0));
}

void ContourEditorScene::mouseMoved(int x, int y, int modif)
{
    Scene::mouseMoved(x, y, modif);
    int lx = m_pts.back().back().x;
    int ly = m_pts.back().back().y;

    double d = sqrt((double)(SQUARE(lx-x) + SQUARE(ly-y)));
    if (d<10) return;
    m_pts.back().push_back(Vec3d(x,y,0));
}

void ContourEditorScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);

    key = tolower(key);

    switch (key)
    {
    case 'd':
        if (m_pts.back().size()>0)
            m_pts.back().resize(m_pts.back().size()-1);
        else
            m_pts.resize(max(1, (int) m_pts.size()-1));
        break;
    case 's':
        saveContoursToFile();
        break;
    case 'p': b_render_pts ^= true;
        break;
    }

}

void ContourEditorScene::reset()
{
    Scene::reset();

    // Fist clear, then resize.
    // This way we make sure that m_pts[0] is cleared.
    m_pts.clear();
    m_pts.resize(1);
}

void ContourEditorScene::saveContoursToFile()
{
    string filename = CONTOUR_FILENAME;
    filename = getBasePath() + filename;
    std::cout << "Saving to " << filename << std::endl;

    FILE* file = fopen(filename.c_str(), "w");
    if (!file) throw "Cannot open <" + filename + "> for writing";

    for (int ci=0; ci<m_pts.size(); ci++) {
        fprintf(file, "---CONTOUR-LINE---\n");
        for (int pi=0; pi<m_pts[ci].size(); pi++) {
            Vec3d &p = m_pts[ci][pi];
            fprintf(file, "%f %f \n", p.x, p.y);
        }
    }

    fclose(file);
}

void ContourEditorScene::loadContoursFromFile(string filename)
{
    FILE* file = fopen(filename.c_str(), "r");
    if (!file) throw "Cannot open <" + filename + "> for reading";

    m_pts.clear();

    char line[1024];
    while (fgets(line, 1023, file)) {
        int len;
        if ((len=strlen(line))<1) continue;
        if (line[len-1] == '\n') line[len-1] = 0;

        if (strcmp(line, CONTOUR_DELIMITER) == 0) {
            m_pts.resize(m_pts.size() + 1);
            continue;
        }

        float x,y;
        sscanf(line, "%f %f", &x, &y);
        m_pts.back().push_back(Vec3d(x,y,0));
    }

    // Delete degenerate contour lines.
    for (int i=0; i<m_pts.size(); ++i) {
        if (m_pts[i].size()<3) {
            m_pts.erase(m_pts.begin()+i);
            --i;
        }
    }

}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new ContourEditorScene);
    }
    catch (std::string exc)
    {
        std::cerr << exc << std::endl;
        return 1;
    }
}
