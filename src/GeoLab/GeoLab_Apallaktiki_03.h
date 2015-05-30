#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include <vector>
#include <string>
#include <MathGeoLib/MathGeoLib.h>
#include <C2DPoint.h>

class Simple3DScene : public vvr::Scene 
{
public:
    Simple3DScene();
    const char* getName() const { return "Simple 3D Scene";}
    void keyEvent(unsigned char key, bool up, int modif) override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;

protected:
    void draw() override;
    void resize() override;

private:
    void drawPolygon();
    void savePolygonToFile();
    void loadPolygonFromFile(string filename);
    void pixelCoordsToSceneCoords(float &x, float &y);

private:
    bool                    b_show_pts;
    vector<C2DPoint>        m_pts;
    vvr::Mesh               m_model;
    vvr::Settings           m_settings;
    vvr::Colour             m_obj_col;
    float                   m_sphere_rad;
    int                     m_style_flag;
};
