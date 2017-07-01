#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vector>
#include <string>
#include <MathGeoLib.h>

using namespace std;
using namespace vvr;
using namespace math;

struct Bone 
{
    std::string name;
    vector<math::Quat> rots;
    vector<double> times;
    vvr::Mesh mesh;
    math::Quat calib_quat;
    bool anim_on;
    double length;
    Bone();
    void animate(float t, float speed = 1);
};

class ArmJointScene : public vvr::Scene
{
public:
    ArmJointScene();
    const char* getName() const { return "Orientation Viewer"; }
    bool idle();
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
