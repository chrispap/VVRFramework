#include "Ex_01_Simple2DScene.h"
#include "Ex_02_ArmMotionScene.h"
#include "Ex_03_PhysicsEngineScene.h"
#include <GL/glut.h>

int main(int argc, char* argv[])
{
    // Needed glutInit to use glut's drawing funcs.
    // Should remove this dep.
    glutInit(&argc, argv); 

    // return vvr::main(argc, argv, new Simple2DScene());
    // return vvr::main(argc, argv, new ArmMotionScene());
    return vvr::main(argc, argv, new PhysicsEngineScene());
}
