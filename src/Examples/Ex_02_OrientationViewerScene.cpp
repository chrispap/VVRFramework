#define  NOMINMAX // Fixes a problem on windows
#include "Ex_02_OrientationViewerScene.h"
#include "utils.h"
#include "canvas.h"
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <MathGeoLib/MathGeoLib.h>

#define FLAG_SHOW_AXES 1

using namespace math;
using namespace vvr;

Bone::Bone() :
    calib_quat(Quat::identity)
{
}

void Bone::animate(float t, float speed)
{
    mesh.setRot(rots.front());

    anim_on = true;

    if (times.empty())
    {
        anim_on = false;
    }
    else
    {
        if (t >= times.back() / speed)
        {
            mesh.setRot(rots.back());
            anim_on = false;
        }
        else
        {
            for (unsigned i = times.size()-1; i>0; i--) {
                if (t >= times[i] / speed) {
                    if (i < rots.size()) mesh.setRot(rots[i]);
                    break;
                }
            }
        }
    }
}

OrientationViewerScene::OrientationViewerScene()
{
    m_style_flag = 0;
    m_settings = Settings(getBasePath() + "config/settings_02.txt");
    m_scene_width = m_settings.getDbl("scene_width");
    m_scene_height = m_settings.getDbl("scene_height");
    m_bgCol = Colour(m_settings.getStr("color_bg"));
    m_bone_col = Colour(m_settings.getStr("color_bone"));
    m_perspective_proj = m_settings.getBool("perspective_proj");
    double def_rot_x = m_settings.getDbl("def_rot_x");
    double def_rot_y = m_settings.getDbl("def_rot_y");
    double def_rot_z = m_settings.getDbl("def_rot_z");
    m_globRotDef = Vec3d(def_rot_x, def_rot_y, def_rot_z);
    m_globRot = m_globRotDef;
    m_scene_dist = m_settings.getDbl("camera_dist");
    load();
    m_anim_speed = m_settings.getDbl("anim_speed");
    m_anim_on = true;
    m_anim_time = 0;
    m_anim_last_update = getSeconds();
}

//! Callbacks - Scene

void OrientationViewerScene::load()
{
    const string objDir = getBasePath() + m_settings.getStr("obj_dir");
    const string objFileBone = getBasePath() +  m_settings.getStr("obj_file_bone");
    Box box;

    // Load & Init: Ulna
    m_humerus.mesh = Mesh(objDir, objFileBone, "");
    m_humerus.length = m_scene_width / 5;
    m_humerus.mesh.setBigSize(m_humerus.length);
    m_humerus.mesh.cornerAlign();
    box = m_humerus.mesh.getBox();
    m_humerus.mesh.move(Vec3d(-box.getYSize()*0.3, -box.getYSize()/2, -box.getZSize()/2));
    m_ulna.mesh = Mesh(m_humerus.mesh);
    m_ulna.mesh.setPos(getUlnaOrigin());

    // Switch mode of execution.
    // Streaming or pre-recorded motion.
    if (m_settings.getBool("mode_playback")) {
        loadRecordedMotion(getBasePath() + m_settings.getStr("data_file_ulna"), m_ulna);
        loadRecordedMotion(getBasePath() + m_settings.getStr("data_file_humerus"), m_humerus);
    }

}

void OrientationViewerScene::draw()
{
    drawAxes();

    // Create style enum
    int s = SOLID;
    if (m_style_flag & FLAG_SHOW_AXES) s |= AXES;

    // Draw objects
    m_ulna.mesh.draw(m_bone_col, (Style) s);
    m_humerus.mesh.draw(m_bone_col, (Style) s);
}

bool OrientationViewerScene::idle()
{
    // Check animation alive
    if (!m_anim_on)
        return false;
    if (!m_settings.getBool("mode_playback"))
        return true;

    // Find current animation time
    m_anim_time += (getSeconds() - m_anim_last_update);
    m_anim_last_update = getSeconds();

    // Animate objects
    m_humerus.animate(m_anim_time, m_anim_speed);
    m_ulna.animate(m_anim_time, m_anim_speed);
    m_ulna.mesh.setPos(getUlnaOrigin());

    // Return
    return m_ulna.anim_on || m_humerus.anim_on;
}

//! Callbacks - UI

void OrientationViewerScene::keyEvent(unsigned char key, bool up, int x, int y, int modif)
{
    Scene::keyEvent(key, up, x, y, modif);

    if (key == ' ')
    {
        m_anim_on = !m_anim_on;
        if (m_anim_on) {
            m_anim_last_update = getSeconds();
        }
    }
    else if (key == 'a')
    {
        m_style_flag ^= FLAG_SHOW_AXES; // show_axes
    }

}

void OrientationViewerScene::arrowEvent(ArrowDir dir, int modif)
{
    Scene::arrowEvent(dir, modif);
}

void OrientationViewerScene::reset()
{
    m_anim_time = 0;
    m_anim_on = true;
    Scene::reset();
}

//! Callbacks - Orientation

void OrientationViewerScene::onDataReceived(const std::string &data)
{
    Vec3d rot;
    std::string rot_str;
    char body_loc[256];
    Bone *bone;

    sscanf(data.c_str(),"%[^:]", body_loc);
    rot_str = data.substr(strlen(body_loc) + 2, 100);

    // If this is a calibration message,
    // just store calibration data.
    if (strcmp("Calibration", body_loc) == 0)
    {
        sscanf(rot_str.c_str(),"%[^:]", body_loc);
        rot_str = rot_str.substr(strlen(body_loc) + 2, 100);
        std::istringstream iss(rot_str);
        iss >> rot.x >> rot.y >> rot.z;

        if (strcmp("Ulna", body_loc) == 0) {
            bone = &m_ulna;
        } else if (strcmp("Humerus", body_loc) == 0) {
            bone = &m_humerus;
        }

        // Store current rotation for this bone.
        // Future rotation will happen relatively to this one.
        bone->calib_quat = getBoneQuaternion(rot);
    }

    // If this a rotation message,
    // rotate the respective body.
    else
    {
        std::istringstream iss(rot_str);
        iss >> rot.x >> rot.y >> rot.z;

        if (strcmp("Ulna", body_loc) == 0) {
            bone = &m_ulna;
        } else if (strcmp("Humerus", body_loc) == 0) {
            bone = &m_humerus;
        }

        // Convert to calibrated frame
        Quat bone_quat;
        bone_quat = (Quat::identity / bone->calib_quat) * getBoneQuaternion(rot);

        // Apply rotation to bone
        vec rot_euler = bone_quat.ToEulerXYZ();
        rot.x = normalizeAngle((RadToDeg( rot_euler.x )));
        rot.y = normalizeAngle((RadToDeg( rot_euler.y )));
        rot.z = normalizeAngle((RadToDeg( rot_euler.z )));

        bone->mesh.setRot(rot);

        if (bone == &m_humerus) {
            m_ulna.mesh.setPos(getUlnaOrigin());
        }
    }

}

//! Helper functions

void OrientationViewerScene::loadRecordedMotion(string filename, Bone &bone)
{
    ifstream file(filename.c_str());
    string line;
    bone.times.clear();
    bone.rots.clear();

    Quat calib_quat = Quat::identity;

    while (getline(file, line))
    {
        int t;
        Vec3d rot;

        if (line.at(0) == '#') {
            string calib_label = "#calibration";
            if (line.compare(0, calib_label.size(), calib_label.c_str())==0) {
                std::istringstream iss(line);
                string trash;
                iss >> trash >> rot.x >> rot.y >> rot.z;
                calib_quat = getBoneQuaternion(Vec3d(rot.x, rot.y, rot.z));
            }
            continue;
        }

        std::istringstream iss(line);
        iss >> t >> rot.x >> rot.y >> rot.z;
        Quat bone_quat = (Quat::identity / calib_quat) * getBoneQuaternion(rot);

        // Apply rotation to bone
        vec rot_euler = bone_quat.ToEulerXYZ();
        rot.x = normalizeAngle((RadToDeg( rot_euler.x )));
        rot.y = normalizeAngle((RadToDeg( rot_euler.y )));
        rot.z = normalizeAngle((RadToDeg( rot_euler.z )));

        bone.times.push_back((double) t / 1000.0);
        bone.rots.push_back(rot);
    }

}

Quat OrientationViewerScene::getBoneQuaternion(const Vec3d &rot)
{
    Quat q = Quat::FromEulerZXY(rot.x, rot.y, rot.z);

    // Transform Android orientation to our system of coords.
    q = q * Quat::RotateY(DegToRad(-90));
    q = q * Quat::RotateZ(DegToRad(90));

    return q;
}

Vec3d OrientationViewerScene::getUlnaOrigin() const
{
    Vec3d rot = m_humerus.mesh.getRot();

    rot.x = DegToRad(rot.x);
    rot.y = DegToRad(rot.y);
    rot.z = DegToRad(rot.z);

    Quat rot_quat = Quat::FromEulerXYZ(rot.x, rot.y, rot.z);
    float3 p = rot_quat.Transform(float3(m_humerus.length, 0, 0));

    return Vec3d(p.x, p.y, p.z);
}

bool OrientationViewerScene::autoDiscoverOnStart()
{
    return !m_settings.getBool("mode_playback") &&
            m_settings.getBool("auto_discover");
}

void OrientationViewerScene::convertDataFile(const std::vector<std::string> filenames)
{
    const unsigned num_bones = filenames.size();
    std::vector<Bone> bones(num_bones);

    // Load files
    for (unsigned i=0; i<num_bones; i++) {
        std::cout << "Loading file: " << filenames[i] << std::endl;
        OrientationViewerScene::loadRecordedMotion(filenames[i], bones[i]);
    }

    // Find max time of the recordings
    float time_end = bones[0].times.back();
    for (unsigned i=1; i<num_bones; i++) {
        if (bones[i].times.back() > time_end) {
            time_end = bones[i].times.back();
        }
    }

    // Open file for writing
    string filename_out = "imus.trc";
    std::ofstream outfile(filename_out.c_str());
    std::cout << "Writing to " << filename_out << std::endl;

    float t_per = 0.010f;
    int num_index = time_end / t_per + 1;
    outfile << "PathFileType\t4\t(X/Y/Z)\timu.trc" << std::endl;
    outfile << "DataRate\tCameraRate\tNumFrames\tNumMarkers\tUnits\tOrigDataRate\tOrigDataStartFrame\tOrigNumFrames" << std::endl;
    outfile << "60\t60\t" << num_index << "\t2\tmm\t60\t1\t" << num_index << "" << std::endl;
    outfile << "Frame#\tTime\thumerus\tulna" << std::endl;
    outfile << "X1\tY1\tZ1\tX2\tY2\tZ2" << std::endl;
    outfile << std::endl;

    // Enforce constant / common sample rate.
    unsigned index=1;
    for (float t=0; t<=time_end; t += t_per)
    {
        outfile << index++ << "\t" << t << "\t";
        for (unsigned bi=0; bi<num_bones; bi++) {
            bones[bi].animate(t);
            outfile <<
            DegToRad(bones[bi].mesh.getRot().x) << "\t" <<
            DegToRad(bones[bi].mesh.getRot().y) << "\t" <<
            DegToRad(bones[bi].mesh.getRot().z) << "\t";
            if (bi<num_bones-2) outfile << "\t";
        }
        outfile << "\n";
    }

    outfile.close();
}
