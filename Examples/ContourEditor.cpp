#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#define APP_TITLE "Contour Editor"

using namespace std;
using namespace vvr;

/**
* @brief The Simple2DScene class
*/
class ContourEditorScene : public vvr::Scene
{
public:
    ContourEditorScene();
    const char* getName() const override { return APP_TITLE; }

protected:
    void draw() override;
    void reset() override;
    void resize() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void arrowEvent(ArrowDir dir, int modif) override;

private:
    void saveContoursToFile();
    void loadContoursFromFile(string filename);
    void pixelCoordsToSceneCoords(float &x, float &y);

private:
    vector<vector<vec> >    m_pts;
    vector<float>           m_heights;
    Canvas2D                m_canvas;
    float                   m_rad;
    bool                    b_render_3D, b_show_pts;
    int                     m_active_contour;
    math::Frustum           m_frustum;
};

#define CONTOUR_FILENAME  "resources/contours/contours.txt"
#define CONTOUR_DELIMITER "CONTOUR-LINE"
#define MIN_POINT_DIST_PIXELS 10

using namespace vvr;
using namespace std;

ContourEditorScene::ContourEditorScene()
{
    m_active_contour = 0;
    m_bg_col = Colour(0x44, 0x44, 0x44);
    m_hide_log = false;
    m_perspective_proj = false;
    b_render_3D = true;
    b_show_pts = true;
    m_pts.resize(1);
    m_heights.resize(1);
    m_heights[0] = 0.1;

    try {
        loadContoursFromFile(getBasePath() + CONTOUR_FILENAME);
    } catch (string exc) {
         std::cerr << exc << endl;
    }
}

void ContourEditorScene::resize()
{
    m_frustum = getFrustum();
}

void ContourEditorScene::draw()
{
    const float height_gain = 1.8;
    const float height_max = height_gain * (*std::max_element(m_heights.begin(), m_heights.end()));

    // Draw all contour lines
    for (int ci = 0; ci < m_pts.size(); ci++)
    {
        for (int pi = 0; pi < m_pts[ci].size(); pi++)
        {
            vec p1 = m_pts[ci][pi];
            vec p2 = m_pts[ci][(pi + 1) % m_pts[ci].size()];

            Colour line_col = Colour::yellow;

            if (ci == m_pts.size() - 1 && pi == m_pts[ci].size() - 1) {
                line_col = Colour::grey;
            }

            if (!b_render_3D)
            {
                LineSeg2D(p1.x, p1.y, p2.x, p2.y, line_col).draw();
                if (b_show_pts) Point2D(p1.x, p1.y, Colour::yellow).draw();
            }
            else
            {
                const bool sel = m_active_contour == ci;
                const double height = height_gain * m_heights[ci];

                vec p1z = p1; p1z.z = height;
                vec p2z = p2; p2z.z = height;

                /* All three colours can be set at once using:
                   --> vvr::Triangle3D::setColourPerVertex(...); */
                
                Colour col_base = Colour::darkGreen;
                Colour col_sel1 = Colour::red;
                Colour col_sel2 = Colour::darkRed;
                Colour col_top; col_top.g = height / height_max * 255;

                Triangle3D t1(
                    p2.x, p2.z, p2.y,
                    p1.x, p1.z, p1.y,
                    p2z.x, p2z.z, p2z.y);

                Triangle3D t2(
                    p1.x, p1.z, p1.y,
                    p1z.x, p1z.z, p1z.y,
                    p2z.x, p2z.z, p2z.y);

                t1.vertex_col[0] = sel ? col_sel2 : col_base;
                t1.vertex_col[1] = sel ? col_sel2 : col_base;
                t1.vertex_col[2] = sel ? col_sel1 : col_top;

                t2.vertex_col[0] = sel ? col_sel2 : col_base;
                t2.vertex_col[1] = sel ? col_sel1 : col_top;
                t2.vertex_col[2] = sel ? col_sel1 : col_top;
                
                t1.draw();
                t2.draw();
            }
        }
    }

}

void ContourEditorScene::mousePressed(int x, int y, int modif)
{
    if (altDown(modif))
    {
        m_pts.resize(m_pts.size() + 1);
        m_heights.resize(m_pts.size());
        m_heights.back() = 0.1;

        float xf = x; float yf = y;
        pixelCoordsToSceneCoords(xf, yf);
        m_pts.back().push_back(vec(xf, yf, 0));
    }
    else if (ctrlDown(modif)) 
    {
        float xf = x; float yf = y;
        pixelCoordsToSceneCoords(xf, yf);
        m_pts.back().push_back(vec(xf, yf, 0));
    }
    else 
    {
        return Scene::mousePressed(x, y, modif);
    }
}

void ContourEditorScene::mouseMoved(int x, int y, int modif)
{
    if (ctrlDown(modif)) 
    {
        float xf = x; float yf = y;
        pixelCoordsToSceneCoords(xf, yf);

        float d, dmin;
        dmin = getSceneWidth() / getViewportWidth() * MIN_POINT_DIST_PIXELS;

        if (!m_pts.empty() && !m_pts.back().empty()) {
            double lx = m_pts.back().back().x;
            double ly = m_pts.back().back().y;
            d = sqrt((double)(Sqrt(lx - xf) + Sqrt(ly - yf)));
        }
        else {
            d = 10000;
        }

        if (d > dmin) {
            m_pts.back().push_back(vec(xf, yf, 0));
        }
    }
    else if (altDown(modif))
    {

    }
    else 
    {
        Scene::mouseMoved(x, y, modif);
        return;
    }
}

void ContourEditorScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);

    key = tolower(key);

    switch (key)
    {
    case 'e':
        b_render_3D = true;
        setFrustum(m_frustum);
        break;
    case 'd':
        if (m_pts.back().size()>0)
            m_pts.back().resize(m_pts.back().size()-1);
        else {
            m_pts.resize(max(1, (int) m_pts.size()-1));
            m_heights.resize(m_pts.size());
        }
        break;
    case 's':
        if (altDown(modif)) saveContoursToFile();
        break;
    case 'p': b_show_pts ^= true;
        break;
    }

}

void ContourEditorScene::arrowEvent(ArrowDir dir, int modif)
{
    if (dir==LEFT) {
        m_active_contour = m_active_contour>0? m_active_contour-1: m_active_contour;
        return;
    }
    if (dir==RIGHT) {
        m_active_contour = m_active_contour<m_pts.size()-1? m_active_contour+1: m_active_contour;
        return;
    }
    if (dir==UP) {
        m_heights[m_active_contour] += 0.1;
        return;
    }
    if (dir==DOWN) {
        m_heights[m_active_contour] -= 0.1;
        if (m_heights[m_active_contour] < 0)
            m_heights[m_active_contour] = 0;
        return;
    }

}

void ContourEditorScene::reset()
{
    m_frustum = getFrustum();
    b_render_3D = false;
    Scene::reset();
}

void ContourEditorScene::saveContoursToFile()
{
    string filename = CONTOUR_FILENAME;
    filename = getBasePath() + filename;
    std::cout << "Saving to " << filename << std::endl;

    FILE* file = fopen(filename.c_str(), "w");
    if (!file) throw "Cannot open <" + filename + "> for writing";

    for (int ci=0; ci<m_pts.size(); ci++) {
        fprintf(file, "%s z=%f \n", CONTOUR_DELIMITER, m_heights[ci]);
        for (int pi=0; pi<m_pts[ci].size(); pi++) {
            vec &p = m_pts[ci][pi];
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

        if (strncmp(line, CONTOUR_DELIMITER, strlen(CONTOUR_DELIMITER)) == 0) {
            float height;
            char trash[256];
            sscanf(line, "%s z=%f", trash, &height);
            m_pts.resize(m_pts.size() + 1);
            m_heights.resize(m_pts.size());
            m_heights.back() = height;
            continue;
        }

        float x,y;
        sscanf(line, "%f %f", &x, &y);
        m_pts.back().push_back(vec(x, y, 0));
    }

    // Delete degenerate contour lines.
    for (int i=0; i<m_pts.size(); ++i) {
        if (m_pts[i].size()<3) {
            m_pts.erase(m_pts.begin()+i);
            --i;
        }
    }

}

void ContourEditorScene::pixelCoordsToSceneCoords(float &x, float &y)
{
    x = getSceneWidth()  / getViewportWidth()  *  x;
    y = getSceneHeight() / getViewportHeight() *  y;
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new ContourEditorScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
        return 1;
    }
}
