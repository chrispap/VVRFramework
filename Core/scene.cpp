#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <MathGeoLib.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <string>
#include <algorithm>
#include <locale>
#include <cmath>
#include <cassert>
#include <QtGui> //gl.h

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

void Scene::enterPixelMode()
{
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-m_screen_width / 2, m_screen_width / 2, -m_screen_height / 2, m_screen_height / 2, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
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
    /* o: Center
    *  X: Rightwards
    *  Y: Upwards
    */
    x -= m_screen_width / 2;
    y -= m_screen_height / 2;
    y = -y;
}

void Scene::pix2mouse(int &x, int &y)
{
    y = -y;
    y += m_screen_height / 2;
    x += m_screen_width / 2;
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
    m_screen_width = w;
    m_screen_height = h;
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
    glViewport(0, 0, m_screen_width, m_screen_height);
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

    int xx=x, yy=y;
    pix2mouse(xx, yy);
    glReadPixels(xx,yy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &m_mouse_depth);
    m_frustum_mlstn = m_frustum;

    if (ctrlDown(modif)) {
        m_mouse_op = 't';
    } else m_mouse_op = 'r';

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
        dv -= right * dx * w / m_screen_width;
        dv -= updir * dy * h / m_screen_height;
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
    glResize(m_screen_width, m_screen_height);
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
