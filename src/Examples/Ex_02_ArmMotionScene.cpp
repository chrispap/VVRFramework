#include "Ex_02_ArmMotionScene.h"
#include "utils.h"
#include "canvas.h"
#include <MathGeoLib/MathGeoLib.h>
#include <iostream>
#include <fstream>
#include <string>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using namespace vvr;
using namespace math;

const char* ArmMotionScene::getName() const
{
    return "Arm Motion Player";
}

ArmMotionScene::ArmMotionScene()
{
    // Read params from configuration file
    m_settings = Settings(getBasePath() + "config/settings_02.txt");
    m_scene_width = m_settings.getDbl("scene_width");
    m_scene_height = m_settings.getDbl("scene_height");
    m_scene_dist =  m_settings.getDbl("scene_dist");
    m_bgCol = Colour(m_settings.getStr("color_bg"));
    m_bone_col = Colour(m_settings.getStr("color_bone"));
    m_tsf = m_settings.getDbl("tsf");
    double def_rot_x = m_settings.getDbl("def_rot_x");
    double def_rot_y = m_settings.getDbl("def_rot_y");
    double def_rot_z = m_settings.getDbl("def_rot_z");
    m_globRotDef = Vec3d(def_rot_x, def_rot_y, def_rot_z);
    m_globRot = m_globRotDef;

    // Load stuff from disk
    load();

    // Start animation just in case.
    m_anim_start_time = getSeconds();
    m_anim_on = true;
}

void ArmMotionScene::loadData(string filename, vector<Vec3d> &rots, vector<double> &times)
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
        rots.push_back(Vec3d(x, y, z));
    }

}

void ArmMotionScene::load()
{
    // Construct paths
    const string objDir = getBasePath() + m_settings.getStr("obj_dir");
    const string objFile = getBasePath() +  m_settings.getStr("obj_file");

    m_bone_width = m_scene_width / 4;
    
    // Load 3D model [Ulna]
    m_ulna.mesh = new Mesh(objDir, objFile, "");
    m_ulna.mesh->setBigSize(m_bone_width);
    m_ulna.mesh->cornerAlign();
    Box box = m_ulna.mesh->getBox();
    m_ulna.mesh->move(Vec3d( -box.getYSize()*0.3, -box.getYSize()/2, -box.getZSize()/2 ));

    // Load 3D model [Humerus]
    m_humerus.mesh = new Mesh(objDir, objFile, "");
    m_humerus.mesh->setBigSize(m_bone_width);
    m_humerus.mesh->cornerAlign();
    box = m_humerus.mesh->getBox();
    m_humerus.mesh->move(Vec3d( -box.getYSize()*0.3, -box.getYSize()/2, -box.getZSize()/2 ));

    // Load motion data
    loadData(getBasePath() + m_settings.getStr("data_file_ulna"), m_ulna.rots, m_ulna.times);
    loadData(getBasePath() + m_settings.getStr("data_file_humerus"), m_humerus.rots, m_humerus.times);
}

void ArmMotionScene::draw()
{
    drawAxes();
    m_ulna.mesh -> draw(m_bone_col, (Style) (SOLID));
    m_humerus.mesh -> draw(m_bone_col, (Style) (SOLID));
}

bool ArmMotionScene::idle()
{
    if (!m_anim_on) return false;

    const double wallTime = getSeconds() - m_anim_start_time;

    bool anim_on_ulna = true;
    bool anim_on_humerus = true;

    // Animate ulna
    if (m_ulna.times.empty()) {
        anim_on_ulna = false;
    } else {
        if (wallTime >= m_ulna.times.back() * m_tsf) {
            m_ulna.mesh->setRot(m_ulna.rots.back());
            anim_on_ulna = false;
        }
        else {
            for (unsigned i = m_ulna.times.size()-1; i>0; i--) {
                if (wallTime >= m_ulna.times[i] * m_tsf) {
                    if (i < m_ulna.rots.size()) m_ulna.mesh->setRot(m_ulna.rots[i]);
                    break;
                }
            }
        }
    }

    // Animate humerus
    // Translate humerus' origin to ulna's end.
    float3x3 M = float3x3::identity;
    M = M * float3x3::RotateX(DegToRad(-90));
    M = M * float3x3::RotateZ(DegToRad(-m_ulna.mesh->getRot().x));
    M = M * float3x3::RotateX(DegToRad(-m_ulna.mesh->getRot().y));
    M = M * float3x3::RotateY(DegToRad( m_ulna.mesh->getRot().z));
    M = M * float3x3::RotateZ(DegToRad(-90));
    float3 p = M.Transform(float3(m_bone_width,0,0));

    m_humerus.mesh->setPos(Vec3d(p.x, p.y, p.z));

    if (m_humerus.times.empty()) {
        anim_on_humerus = false;
    } else {
        if (wallTime >= m_humerus.times.back() * m_tsf) {
            m_humerus.mesh->setRot(m_humerus.rots.back());
            anim_on_humerus = false;
        }
        else {
            for (unsigned i = m_humerus.times.size()-1; i>0; i--) {
                if (wallTime >= m_humerus.times[i] * m_tsf) {
                    if (i < m_humerus.rots.size()) m_humerus.mesh->setRot(m_humerus.rots[i]);
                    break;
                }
            }
        }
    }

    return anim_on_ulna || anim_on_humerus;
}
