#include "scene.h"

#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <string>
#include <algorithm>
#include <locale>
#include <cmath>
#include <QtOpenGL>

using namespace vvr;
using namespace std;

Scene::Scene() : perspective_proj(false), globRotDef(0,0,0)
{
    globRot = globRotDef;
}

const char* Scene::getName() const
{
    return "OpenGL Scene";
}

void Scene::drawAxes()
{
    glBegin(GL_LINES);
    //[X]
    glColor3ub(0xFF, 0, 0);
    glVertex3f(0,0,0);
    glVertex3f(2.0*scene_width, 0, 0);
    //[Y]
    glColor3f(0, 0xFF, 0);
    glVertex3f(0,0,0);
    glVertex3f(0, 10.0*scene_height, 0);
    //[Z]
    glColor3f(0, 0, 0xFF);
    glVertex3f(0,0,0);
    glVertex3f(0, 0, 10.0*camera_dist);

    glEnd();
}

/* OpenGL Callbacks */
void Scene::GL_Init()
{
    GLfloat light_position[] = {    0,    0, -200,  1};
    GLfloat ambientLight[]   = {  0.5,  0.5,  0.5,  1};
    GLfloat diffuseLight[]   = {  0.8,  0.8,  0.8,  1};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);
    //glEnable(GL_BLEND);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Scene::GL_Resize(int w, int h)
{
    screen_width = w;
    screen_height = h;

    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if(perspective_proj)
    {
//        gluPerspective(
//            10.0,
//            (float)w/h,
//            camera_dist * 0.002,
//            camera_dist * 20
//        );
    }
    else
    {
        // Keep constant aspect ratio
        scene_height = scene_width * h/w;

        glOrtho(
            -scene_width / 2,
            scene_width / 2,

            -scene_height / 2,
            scene_height / 2,

            camera_dist * -2, // -Z look to the user! Near should be negative!
            camera_dist * 2   // Far should be positive!
        );
    }

}

void Scene::GL_Render()
{
    glClearColor(bgCol.r/255.0, bgCol.g/255.0, bgCol.b/255.0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Push the scene to the back (far from camera)
    glTranslatef(0,0,-camera_dist);

    // Apply global rotation & draw
    glRotatef(globRot.x, 1, 0, 0);
    glRotatef(globRot.y, 0, 1, 0);
    glRotatef(globRot.z, 0, 0, 1);
    draw();
}

/* UI Callbacks */
void Scene::keyEvent (unsigned char key,  bool up, int x, int y, int modif)
{
    if (up) return;

    int ctrl  =  modif & 0x01;
    int shift =  modif & 0x02;

    switch (isprint(key)? tolower(key): key) {
    case '0' : globRot = globRotDef; break;
    case 'r' : reset();
    }

}

void Scene::arrowEvent (ArrowDir dir, int modif)
{
    int ctrl  =  modif & 0x01;
    int shift =  modif & 0x02;

}

void Scene::mousePressed(int x, int y, int modif)
{
    mouselastX = x;
    mouselastY = y;
}

void Scene::mouseMoved(int x, int y, int modif)
{
    int dx = x - mouselastX;
    int dy = y - mouselastY;

    globRot.x += dy;
    globRot.y += dx;

    globRot.x = fmod(globRot.x, 360.0);
    while (globRot.x < 0) globRot.x += 360;

    globRot.y = fmod(globRot.y, 360.0);
    while (globRot.y < 0) globRot.y += 360;

    mouselastX = x;
    mouselastY = y;
}

void Scene::mouseWheel(int dir, int modif)
{
    camera_dist += 0.2*dir;

    if (camera_dist < 0.01) camera_dist = 0.01;
}

/* Utils */
void Scene::enterPixelMode()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, screen_width, screen_height, 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
}

void Scene::returnFromPixelMode()
{
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}
