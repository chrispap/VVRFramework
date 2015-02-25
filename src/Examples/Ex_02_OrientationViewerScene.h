#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include <vector>
#include <string>
#include <MathGeoLib/MathGeoLib.h>

struct Bone {
    std::string         name;
    vector<vvr::Vec3d>  rots;
    vector<double>      times;
    vvr::Mesh           mesh;
    math::Quat          calib_quat;
    bool                anim_on;
    double              length;
    Bone();
    void animate(float t, float speed=1);
};

class OrientationViewerScene : public vvr::Scene
{
public:
    OrientationViewerScene();
    const char* getName() const { return "Orientation Viewer";}
    bool idle();
    void onDataReceived(const string &data);
    bool autoDiscoverOnStart();
    void keyEvent(unsigned char key, bool up, int x, int y, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;
    void reset() override;

public:
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
    static Quat getBoneQuaternion(const vvr::Vec3d &rot);

protected:
    void draw() override;
    void load();

private:
    /**
     * @return The 3D position (in world coords) of humerus' origin.
     * It is actually the ending point of Ulna.
     */
    vvr::Vec3d getUlnaOrigin() const;

private:
    vvr::Settings   m_settings;
    vvr::Colour     m_bone_col;
    int             m_style_flag;
    Bone            m_ulna, m_humerus;
    double          m_anim_time, m_anim_last_update;
    double          m_anim_speed;
    bool            m_anim_on;
};
