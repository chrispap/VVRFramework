#include "templatescene.h"
#include "utils.h"
#include "canvas.h"

#include <iostream>
#include <fstream>
#include <string>
#include <Mathgeolib/MathGeoLib.h>

using std::vector;
using std::string;
using std::cout;
using std::endl;

TemplateScene::TemplateScene() :
    m_settings(getExePath() + "../../config/settings.txt")
{
    // Read params from configuration file
    camera_dist = m_settings.getDbl("camera_dist");
    scene_width = m_settings.getDbl("scene_width");
    scene_height = m_settings.getDbl("scene_height");
    bgCol = vvr::ColRGB(m_settings.getStr("color_bg"));
    perspective_proj = m_settings.getBool("perspective_proj");
    tsf = m_settings.getDbl("tsf");
    double def_rot_x = m_settings.getDbl("def_rot_x");
    double def_rot_y = m_settings.getDbl("def_rot_y");
    double def_rot_z = m_settings.getDbl("def_rot_z");
    globRotDef = vvr::Vec3d(def_rot_x, def_rot_y, def_rot_z);
    globRot = globRotDef;

    // Load stuff from disk
    load();

    // Start animation just in case.
    m_anim_start_time = getSeconds();
    m_anim_on = true;
}

void TemplateScene::loadData(string filename, vector<vvr::Vec3d> &rots, vector<double> &times)
{
    ifstream file(filename.c_str());
    string line;
    times.clear();
    rots.clear();

    while (getline(file, line))
    {
        int t;
        double x,y,z;
        sscanf(line.c_str(), "%d %lf %lf %lf", &t, &x, &y, &z);
        x *= (180.0/3.14);
        y *= (180.0/3.14);
        z *= (180.0/3.14);
        times.push_back((double) t / 1000.0);
        rots.push_back(vvr::Vec3d(x, y, z));
    }

}

void TemplateScene::load()
{
    // Construct paths
    const string objDir = getBasePath() + m_settings.getStr("obj_dir");
    const string objFile = getBasePath() +  m_settings.getStr("obj_file");

    m_bone_width = scene_width / 4;
    
    // Load 3D model [Ulna]
    ulna = new vvr::Mesh(objDir, objFile, "");
    ulna->setBigSize(m_bone_width);
    ulna->cornerAlign();
    vvr::Box box = ulna->getBox();
    ulna->move(vvr::Vec3d( -box.getYSize()*0.3, -box.getYSize()/2, -box.getZSize()/2 ));

    // Load 3D model [Humerus]
    humerus = new vvr::Mesh(objDir, objFile, "");
    humerus->setBigSize(m_bone_width);
    humerus->cornerAlign();
    box = humerus->getBox();
    humerus->move(vvr::Vec3d( -box.getYSize()*0.3, -box.getYSize()/2, -box.getZSize()/2 ));

    // Load motion data
    loadData(getBasePath() + m_settings.getStr("data_file_ulna"), m_rots_ulna, m_times_ulna);
    loadData(getBasePath() + m_settings.getStr("data_file_humerus"), m_rots_humerus, m_times_humerus);
}

void TemplateScene::draw()
{
    drawAxes();
    ulna    -> draw(vvr::ColRGB(0x66, 0x00, 0x66), (vvr::Style) (vvr::SOLID));
    humerus -> draw(vvr::ColRGB(0x66, 0x00, 0x66), (vvr::Style) (vvr::SOLID));
}

bool TemplateScene::idle()
{
    if (!m_anim_on) return false;

    const double wallTime = getSeconds() - m_anim_start_time;

    bool anim_on_ulna = true;
    bool anim_on_humerus = true;

    // Animate ulna
    if (m_times_ulna.empty()) {
        anim_on_ulna = false;
    } else {
        if (wallTime >= m_times_ulna.back() * tsf) {
            ulna->setRot(m_rots_ulna.back());
            anim_on_ulna = false;
        }
        else {
            for (unsigned i = m_times_ulna.size()-1; i>0; i--) {
                if (wallTime >= m_times_ulna[i] * tsf) {
                    if (i < m_rots_ulna.size()) ulna->setRot(m_rots_ulna[i]);
                    break;
                }
            }
        }
    }

    // Animate humerus
    // Translate humerus' origin to ulna's end.
    math::float3x3 M = math::float3x3::identity;
    M = M * math::float3x3::RotateX(math::DegToRad(-90));
    M = M * math::float3x3::RotateZ(math::DegToRad(-ulna->getRot().x));
    M = M * math::float3x3::RotateX(math::DegToRad(-ulna->getRot().y));
    M = M * math::float3x3::RotateY(math::DegToRad( ulna->getRot().z));
    M = M * math::float3x3::RotateZ(math::DegToRad(-90));
    math::float3 p = M.Transform(math::float3(m_bone_width,0,0));

    humerus->setPos(vvr::Vec3d(p.x, p.y, p.z));

    if (m_times_humerus.empty()) {
        anim_on_humerus = false;
    } else {
        if (wallTime >= m_times_humerus.back() * tsf) {
            humerus->setRot(m_rots_humerus.back());
            anim_on_humerus = false;
        }
        else {
            for (unsigned i = m_times_humerus.size()-1; i>0; i--) {
                if (wallTime >= m_times_humerus[i] * tsf) {
                    if (i < m_rots_humerus.size()) humerus->setRot(m_rots_humerus[i]);
                    break;
                }
            }
        }
    }

    return anim_on_ulna || anim_on_humerus;
}

int main(int argc, char* argv[])
{
    TemplateScene *scene = new TemplateScene();
    return  vvr::main(argc, argv, scene);
}
