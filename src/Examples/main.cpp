#include "Ex_01_Simple2DScene.h"
#include "Ex_02_OrientationViewerScene.h"
#include "Ex_03_PhysicsEngineScene.h"
#include <GL/glut.h>

int main(int argc, char* argv[])
{
    // Needed glutInit to use glut's drawing funcs.
    glutInit(&argc, argv);  // Should remove this dep.

    /* 3 Example scenes using VVR Framework */
    return vvr::main(argc, argv, new Simple2DScene());
    return vvr::main(argc, argv, new OrientationViewerScene);
    return vvr::main(argc, argv, new PhysicsEngineScene());
}
