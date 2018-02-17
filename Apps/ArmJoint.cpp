#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/utils.h>
#include <vvr/drawing.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <string>
#include <MathGeoLib.h>

using namespace std;
using namespace vvr;
using namespace math;

struct Bone
{
    Bone();
    std::string name;
    vector<math::Quat> rots;
    vector<double> times;
    vvr::Mesh::Ptr mesh;
    math::Quat calib_quat;
    bool anim_on;
    double length;
    void animate(float t, float speed = 1);
};

class ArmJointScene : public vvr::Scene
{
public:
    ArmJointScene();
    const char* getName() const override { return "Orientation Viewer"; }
    bool idle() override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;
    void reset() override;

    /**
    * @brief convertDataFile
    * @param filenames
    */
    static void convertDataFile(const std::vector<std::string> filenames);

    /**
    * @brief Load motion from file.
    * @param filename
    * @param bone The struct to store the motion.
    */
    static void loadRecordedMotion(const string filename, Bone &bone);

    /**
    * @param rot Euler angles in radians
    * @return Rotation matrix for these euler angles
    */
    static Quat getBoneQuaternion(const vec &rot);

protected:
    void draw() override;
    void resize() override;
    void load();

private:
    /**
    * @return The 3D position (in world coords) of humerus' origin.
    * It is actually the ending point of Radius.
    */
    vec getRadiusOrigin() const;

private:
    vvr::Colour m_bone_col;
    int m_style_flag;
    Bone m_radius, m_humerus;
    double m_anim_time, m_anim_last_update;
    double m_anim_speed;
    bool m_anim_on;
};

#define MODE_PLAYBACK true
#define FILE_RADIUS "resources/data/LineTest/Radius.txt"
#define FILE_HUMERUS "resources/data/LineTest/Humerus.txt"

#define FLAG_SHOW_AXES      1
#define FLAG_RENDER_SOLID   2
#define FLAG_RENDER_WIRE    4

using namespace std;
using namespace vvr;
using namespace math;

Bone::Bone() : calib_quat(Quat::identity)
{

}

void Bone::animate(float t, float speed)
{
    mesh->setTransform(rots.front().ToFloat3x4());

    anim_on = true;

    if (times.empty())
    {
        anim_on = false;
    }
    else
    {
        if (t >= times.back() / speed)
        {
            mesh->setTransform(rots.back().ToFloat3x4());
            anim_on = false;
        }
        else
        {
            for (unsigned i = times.size() - 1; i > 0; i--) {
                if (t >= times[i] / speed) {
                    if (i < rots.size()) {
                        mesh->setTransform(rots.at(i).ToFloat3x4());
                    }
                    break;
                }
            }
        }
    }
}

ArmJointScene::ArmJointScene()
{
    m_style_flag = FLAG_RENDER_SOLID;
    m_bg_col = Colour("768E77");
    m_bone_col = Colour("8A7F4D");
    m_perspective_proj = true;
    load();
    m_anim_speed = 1;
    m_anim_on = true;
    m_anim_time = 0;
    m_anim_last_update = getSeconds();
}

//! Callbacks - Scene

void ArmJointScene::load()
{
    const string objFileBone = getBasePath() + "resources/obj/bone.obj";

    // Load 3D models Humerus
    m_humerus.mesh = Mesh::Make(objFileBone);
    m_radius.mesh = Mesh::Make(*m_humerus.mesh);

    // Switch mode of execution: {Live Streaming, Playback}
    loadRecordedMotion(getBasePath() + FILE_RADIUS, m_radius);
    loadRecordedMotion(getBasePath() + FILE_HUMERUS, m_humerus);
}

void ArmJointScene::resize()
{
    m_humerus.length = getSceneWidth() / 5;
    m_humerus.mesh->setBigSize(m_humerus.length);
    m_humerus.mesh->cornerAlign();
    AABB aabb = m_humerus.mesh->getAABB();
    m_humerus.mesh->move(vec(-aabb.Size().y * 0.3, -aabb.Size().y / 2, -aabb.Size().z / 2));
    m_radius.length = getSceneWidth() / 5;
    m_radius.mesh->setBigSize(m_radius.length);
    m_radius.mesh->cornerAlign();
    m_radius.mesh->move(vec(-aabb.Size().y * 0.3, -aabb.Size().y / 2, -aabb.Size().z / 2));
    float3x4 transform = m_radius.mesh->getTransform();
    transform.SetTranslatePart(getRadiusOrigin());
    m_radius.mesh->setTransform(transform);
}

void ArmJointScene::draw()
{
    drawAxes();

    // Create style enum
    int s = 0;
    if (m_style_flag & FLAG_RENDER_SOLID) s |= SOLID;
    if (m_style_flag & FLAG_RENDER_WIRE) s |= WIRE;
    if (m_style_flag & FLAG_SHOW_AXES) s |= AXES;

    // Draw objects
    m_radius.mesh->draw(m_bone_col, (Style)s);
    m_humerus.mesh->draw(m_bone_col, (Style)s);
}

bool ArmJointScene::idle()
{
    // Check animation alive
    if (!m_anim_on) return false;

    // Find current animation time
    m_anim_time += (getSeconds() - m_anim_last_update);
    m_anim_last_update = getSeconds();

    // Animate objects
    m_humerus.animate(m_anim_time, m_anim_speed);
    m_radius.animate(m_anim_time, m_anim_speed);
    float3x4 transform = m_radius.mesh->getTransform();
    transform.SetTranslatePart(getRadiusOrigin());
    m_radius.mesh->setTransform(transform);

    return m_radius.anim_on || m_humerus.anim_on;
}

//! Callbacks - UI

void ArmJointScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);

    if (key == ' ')
    {
        m_anim_on = !m_anim_on;
        if (m_anim_on) {
            m_anim_last_update = getSeconds();
        }
    }
    else if (key == 'a') m_style_flag ^= FLAG_SHOW_AXES;
    else if (key == 's') m_style_flag ^= FLAG_RENDER_SOLID;
    else if (key == 'w') m_style_flag ^= FLAG_RENDER_WIRE;
}

void ArmJointScene::arrowEvent(ArrowDir dir, int modif)
{
    Scene::arrowEvent(dir, modif);
}

void ArmJointScene::reset()
{
    m_anim_time = 0;
    m_anim_on = true;
    m_style_flag = FLAG_RENDER_SOLID;
    Scene::reset();
}

//! Helper functions

void ArmJointScene::loadRecordedMotion(string filename, Bone &bone)
{
    ifstream file(filename.c_str());

    if (!file.is_open())
        throw "Cannot open <" + filename + ">";

    string line;
    bone.times.clear();
    bone.rots.clear();

    Quat calib_quat = Quat::identity;

    while (getline(file, line))
    {
        int t;
        vec rot;

        if (line.at(0) == '#') {
            string calib_label = "#calibration";
            if (line.compare(0, calib_label.size(), calib_label.c_str()) == 0) {
                std::istringstream iss(line);
                string trash;
                iss >> trash >> rot.x >> rot.y >> rot.z;
                calib_quat = getBoneQuaternion(vec(rot.x, rot.y, rot.z));
            }
            continue;
        }

        std::istringstream iss(line);
        iss >> t >> rot.x >> rot.y >> rot.z;
        Quat bone_quat = (Quat::identity / calib_quat) * getBoneQuaternion(rot);
        bone.times.push_back((double)t / 1000.0);
        bone.rots.push_back(bone_quat);
    }

}

Quat ArmJointScene::getBoneQuaternion(const vec &rot)
{
    Quat q = Quat::FromEulerZXY(rot.x, rot.y, rot.z);

    // Transform Android orientation to our system of coords.
    q = q * Quat::RotateY(DegToRad(-90));
    q = q * Quat::RotateZ(DegToRad(90));

    return q;
}

vec ArmJointScene::getRadiusOrigin() const
{
    float3 humerus_end = float3(m_humerus.length, 0, 0); // asuming x the largest dimension
    return m_humerus.mesh->getTransform().TransformPos(humerus_end);
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::main_with_scene(argc, argv, new ArmJointScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
    }
    return 1;
}
