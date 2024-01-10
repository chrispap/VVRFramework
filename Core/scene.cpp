#include "Math/float2.h"
#include <MathGeoLib.h>
#include <QtGui> //gl.h
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vvr/drawing.h>
#include <vvr/scene.h>
#include <vvr/utils.h>

using namespace vvr;
using namespace std;
using namespace math;

#define VVR_FOV_MAX 160
#define VVR_FOV_MIN 2
#define DEFAULT_CAM_DIST 100

/*--------------------------------------------------------------------------------------*/
Scene::Scene()
{
    m_bg_col = vvr::white;
    m_perspective_proj = false;
    m_fullscreen = false;
    m_create_menus = false;
    m_show_log = false;
    m_show_sliders = false;
    m_fov = 66;
    m_first_resize = true;
    m_2d_center = math::float2{0.0f, 0.0f};
    m_arrow_state[0] = false;
    m_arrow_state[1] = false;
    m_arrow_state[2] = false;
    m_arrow_state[3] = false;
    setCameraPos(vec(0, 0, DEFAULT_CAM_DIST));
}

void Scene::reset()
{
    setCameraPos(vec(0, 0, DEFAULT_CAM_DIST));
}

void Scene::resize()
{

}

void Scene::drawAxes()
{
    Axes(2.0f * getSceneWidth()).draw();
}

void Scene::enter2dMode(math::float2 center, math::float2 size)
{
    // m_2d_center = center;
    // m_2d_size = size;

    const float halfWidth = size.x / 2.0;
    const float halfHeight = size.y / 2.0;
    const float left = center.x - halfWidth;
    const float right = center.x + halfWidth;
    const float bottom = center.y - halfHeight;
    const float top = center.y + halfHeight;
    const float nearPlane = 1;
    const float farPlane = -1;

    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(left, right, bottom, top, nearPlane, farPlane);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
}

void Scene::enter2dMode(float cx, float cy, float w, float h)
{
    enter2dMode(float2(cx, cy), float2(w, h));
}

void Scene::enterPixelMode()
{
    enter2dMode(0, 0, m_viewport_width, m_viewport_height);
}

void Scene::exitPixelMode()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void Scene::mouse2pix(int &x, int &y)
{
    /// [Qt Mouse event coordinates]:
    /// - origin in the top-left
    /// - x increases to the right
    /// - y increases to the bottom

    /// [VVR Pixel coordinates]:
    /// - origin in the center
    /// - x increases to the right
    /// - y increases to the top

    x = x - (m_viewport_width / 2.);
    y = -(y - (m_viewport_height / 2.));
}

void Scene::pix2mouse(int &x, int &y)
{
    x = x + (m_viewport_width / 2.);
    y = -y + (m_viewport_height / 2.);
}

void Scene::setCameraPos(const vec &pos)
{
    vec up(0, 1, 0);
    vec front = pos.Neg().Normalized();
    if (fabs(up.Dot(front)) > 0.9f) up.Set(1, 0, 0);
    vec left = up.Cross(front);
    up = front.Cross(left).Normalized();
    m_frustum.SetFrame(pos, front, up);
    m_frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
}

Ray Scene::unproject(int x, int y)
{
    return m_frustum.UnProject(
        static_cast<float>(x) / getViewportWidth() * 2,
        static_cast<float>(y) / getViewportHeight() * 2);
}

/*---[OpenGL]---------------------------------------------------------------------------*/
static void glInfo()
{
    printf("\n=== VVR-Framework 2018 ===========\n");
    printf(" %s\n", glGetString(GL_VERSION));
    printf(" %s\n", glGetString(GL_VENDOR));
    printf(" %s\n", glGetString(GL_RENDERER));
    printf("==================================\n\n");
    fflush(nullptr);
}

void Scene::glInit()
{
    float lz = DEFAULT_CAM_DIST * 3.0f;
    static GLfloat light_position[] = { 0.00f, lz, lz, 1.0f };
    static GLfloat ambientLight[]   = { 0.75f, 0.75f, 0.75f, 1.0f };
    static GLfloat diffuseLight[]   = { 0.75f, 0.75f, 0.75f, 1.0f };
    static GLfloat specularLight[]  = { 0.85f, 0.85f, 0.85f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glInfo();
}

void Scene::glResize(int w, int h)
{
    m_viewport_width = w;
    m_viewport_height = h;
    float cam_dist = m_frustum.Pos().Length();

    /* Set frustum with following modes:
     * - Perspective
     * - Orthographic */
    if (m_perspective_proj)
    {
        const float n = cam_dist * 0.1;
        const float f = cam_dist * 2.0;
        m_frustum.SetViewPlaneDistances(n, f);
        m_frustum.SetVerticalFovAndAspectRatio(DegToRad(m_fov), ((float)w/h));
        m_scene_width = m_frustum.NearPlaneWidth() / 0.1;
        m_scene_height = m_frustum.NearPlaneHeight() / 0.1;
    }
    else
    {
        const float n = -cam_dist * 2;
        const float f = cam_dist * 2;
        m_scene_width = cam_dist / 2;
        m_scene_height = m_scene_width / ((float)w/h);
        m_frustum.SetOrthographic(m_scene_width, m_scene_height);
        m_frustum.SetViewPlaneDistances(n, f);
    }

    m_frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
    m_axes.setSize(getSceneWidth());
    resize();
    m_first_resize = false;
}

void Scene::glRender()
{
    //---[Prepare matrices]---
    float4x4 pjm = m_frustum.ProjectionMatrix();
    float4x4 mvm = m_frustum.ViewMatrix();
    pjm.Transpose(); // Colunm major for OpenGL.
    mvm.Transpose(); // Colunm major for OpenGL.

    //---[Render scene]---
    glClearColor(m_bg_col.r / 255.0, m_bg_col.g / 255.0, m_bg_col.b / 255.0, 1);
    glViewport(0, 0, m_viewport_width, m_viewport_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(pjm.ptr());
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(mvm.ptr());
    draw();
}

/*---[Events]---------------------------------------------------------------------------*/
void Scene::keyEvent(unsigned char key, bool up, int modif)
{
    if (up) return;

    switch (isprint(key) ? tolower(key) : key) {
    case 'r': this->reset(); break;
    case '2': setCameraPos(vec(0, -DEFAULT_CAM_DIST, 0)); break;
    case '4': setCameraPos(vec(-DEFAULT_CAM_DIST, 0, 0)); break;
    case '6': setCameraPos(vec(DEFAULT_CAM_DIST, 0, 0)); break;
    case '8': setCameraPos(vec(0, DEFAULT_CAM_DIST, 0)); break;
    case '5': setCameraPos(vec(0, 0, DEFAULT_CAM_DIST)); break;
    }
}

void Scene::arrowEvent(ArrowDir dir, int modif)
{

}

void Scene::mousePressed(int x, int y, int modif)
{
    m_mouse_x = x;
    m_mouse_y = y;
    m_frustum_mlstn = m_frustum;

    if (ctrlDown(modif)) {
        m_mouse_op = 't';
    } else m_mouse_op = 'r';

    pix2mouse(x, y);
    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &m_mouse_depth);

    cursorGrab();
}

void Scene::mouseMoved(int x, int y, int modif)
{
    const math::vec updir = m_frustum.Up();
    const math::vec right = m_frustum.WorldRight();

    if (m_mouse_op=='r')
    {
        const int dx = x - m_mouse_x;
        const int dy = y - m_mouse_y;
        m_mouse_x = x;
        m_mouse_y = y;
        /* Rotate scene */
        float rot_x = math::DegToRad(-vvr::normalize_deg((float)dx/2));
        float rot_y = math::DegToRad( vvr::normalize_deg((float)dy/2));
        math::float3x3 transform = math::float3x3::identity;
        transform = transform * float3x3::RotateAxisAngle(right, rot_y);
        transform = transform * float3x3::RotateAxisAngle(updir, rot_x);
        m_frustum.Transform(transform);
        m_frustum.SetPos(m_frustum.Pos());
    }
    else if (m_mouse_op=='t')
    {
        const int dx = x - m_mouse_x;
        const int dy = y - m_mouse_y;
        /* Translate scene */
        float w = m_scene_width;
        float h = m_scene_height;
        vec dv = vec::zero;
        dv -= right * dx * w / m_viewport_width;
        dv -= updir * dy * h / m_viewport_height;
        m_frustum.SetPos(m_frustum_mlstn.Pos() + dv);
    }
    else
    {
        vvr_msg("Should not reach.");
        assert(0);
    }
}

void Scene::mouseHovered(int x, int y, int modif)
{

}

void Scene::mouseReleased(int x, int y, int modif)
{
    m_mouse_op = '\0';
    cursorShow();
}

void Scene::mouseWheel(int dir, int modif)
{
    m_fov += -4.0 * dir;
    if (m_fov < VVR_FOV_MIN) m_fov = VVR_FOV_MIN;
    else if (m_fov > VVR_FOV_MAX) m_fov = VVR_FOV_MAX;
    glResize(m_viewport_width, m_viewport_height);
}

void Scene::sliderChanged(int slider_id, float val)
{

}

void Scene::setSliderVal(int slider_id, float val)
{
    if (slider_id > 5 || slider_id < 0) return;
    if (val > 1) val = 1;
    else if (val < 0) val = 0;
    //TODO: Connect scene object backwards to window
    //      in order to be able to change
    //      slider values from the scene.
}
/*--------------------------------------------------------------------------------------*/
